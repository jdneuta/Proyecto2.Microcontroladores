#include "bme280_lib.h"
#include "i2c.h"
#include "config.h"

/* Registros principales del BME280 */
#define BME280_CHIP_ID          0x60
#define BME280_REG_DIG_T1       0x88
#define BME280_REG_DIG_P1       0x8E
#define BME280_REG_DIG_H1       0xA1
#define BME280_REG_DIG_H2       0xE1
#define BME280_REG_CHIPID       0xD0
#define BME280_REG_RESET        0xE0
#define BME280_REG_CTRL_HUM     0xF2
#define BME280_REG_STATUS       0xF3
#define BME280_REG_CTRL_MEAS    0xF4
#define BME280_REG_CONFIG       0xF5
#define BME280_REG_PRESS_MSB    0xF7

/* Estructura con los datos de calibracion internos del sensor. */
typedef struct
{
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
} BME280_Calib_t;

static BME280_Calib_t bme280_calib;
static int32_t bme280_t_fine = 0;
static uint8_t bme280_ok = 0;

/* Variables crudas leidas del sensor. */
static int32_t adc_T = 0;
static int32_t adc_P = 0;
static int32_t adc_H = 0;

/* -------------------------------------------------------------
 * Funciones I2C basicas
 * ------------------------------------------------------------- */
static void BME280_Write8(uint8_t reg, uint8_t data)
{
    I2C_Start();
    I2C_Write((BME280_ADDRESS << 1) | 0);   /* Escritura */
    I2C_Write(reg);
    I2C_Write(data);
    I2C_Stop();
}

static uint8_t BME280_Read8(uint8_t reg)
{
    uint8_t value;

    I2C_Start();
    I2C_Write((BME280_ADDRESS << 1) | 0);   /* Escritura */
    I2C_Write(reg);
    I2C_Restart();
    I2C_Write((BME280_ADDRESS << 1) | 1);   /* Lectura */
    value = I2C_Read();
    I2C_Nack();
    I2C_Stop();

    return value;
}

static uint16_t BME280_Read16LE(uint8_t reg)
{
    uint8_t lsb;
    uint8_t msb;

    I2C_Start();
    I2C_Write((BME280_ADDRESS << 1) | 0);
    I2C_Write(reg);
    I2C_Restart();
    I2C_Write((BME280_ADDRESS << 1) | 1);
    lsb = I2C_Read();
    I2C_Ack();
    msb = I2C_Read();
    I2C_Nack();
    I2C_Stop();

    return ((uint16_t)msb << 8) | lsb;
}

static void BME280_ReadBytes(uint8_t reg, uint8_t *buffer, uint8_t len)
{
    uint8_t i;

    I2C_Start();
    I2C_Write((BME280_ADDRESS << 1) | 0);
    I2C_Write(reg);
    I2C_Restart();
    I2C_Write((BME280_ADDRESS << 1) | 1);

    for(i = 0; i < len; i++)
    {
        buffer[i] = I2C_Read();

        if(i < (len - 1))
        {
            I2C_Ack();
        }
        else
        {
            I2C_Nack();
        }
    }

    I2C_Stop();
}

static int16_t BME280_SignExtend12(uint16_t value)
{
    if(value & 0x0800)
    {
        value |= 0xF000;
    }

    return (int16_t)value;
}

/* -------------------------------------------------------------
 * Calibracion e inicializacion
 * ------------------------------------------------------------- */
static void BME280_ReadCalibration(void)
{
    bme280_calib.dig_T1 = BME280_Read16LE(BME280_REG_DIG_T1 + 0);
    bme280_calib.dig_T2 = (int16_t)BME280_Read16LE(BME280_REG_DIG_T1 + 2);
    bme280_calib.dig_T3 = (int16_t)BME280_Read16LE(BME280_REG_DIG_T1 + 4);

    bme280_calib.dig_P1 = BME280_Read16LE(BME280_REG_DIG_P1 + 0);
    bme280_calib.dig_P2 = (int16_t)BME280_Read16LE(BME280_REG_DIG_P1 + 2);
    bme280_calib.dig_P3 = (int16_t)BME280_Read16LE(BME280_REG_DIG_P1 + 4);
    bme280_calib.dig_P4 = (int16_t)BME280_Read16LE(BME280_REG_DIG_P1 + 6);
    bme280_calib.dig_P5 = (int16_t)BME280_Read16LE(BME280_REG_DIG_P1 + 8);
    bme280_calib.dig_P6 = (int16_t)BME280_Read16LE(BME280_REG_DIG_P1 + 10);
    bme280_calib.dig_P7 = (int16_t)BME280_Read16LE(BME280_REG_DIG_P1 + 12);
    bme280_calib.dig_P8 = (int16_t)BME280_Read16LE(BME280_REG_DIG_P1 + 14);
    bme280_calib.dig_P9 = (int16_t)BME280_Read16LE(BME280_REG_DIG_P1 + 16);

    bme280_calib.dig_H1 = BME280_Read8(BME280_REG_DIG_H1);
    bme280_calib.dig_H2 = (int16_t)BME280_Read16LE(BME280_REG_DIG_H2);
    bme280_calib.dig_H3 = BME280_Read8(BME280_REG_DIG_H2 + 2);

    bme280_calib.dig_H4 = BME280_SignExtend12(((uint16_t)BME280_Read8(BME280_REG_DIG_H2 + 3) << 4) |
                                              (BME280_Read8(BME280_REG_DIG_H2 + 4) & 0x0F));

    bme280_calib.dig_H5 = BME280_SignExtend12(((uint16_t)BME280_Read8(BME280_REG_DIG_H2 + 5) << 4) |
                                              (BME280_Read8(BME280_REG_DIG_H2 + 4) >> 4));

    bme280_calib.dig_H6 = (int8_t)BME280_Read8(BME280_REG_DIG_H2 + 6);
}

uint8_t BME280_Init(void)
{
    uint8_t id;
    uint16_t timeout;

    bme280_ok = 0;

    id = BME280_Read8(BME280_REG_CHIPID);
    if(id != BME280_CHIP_ID)
    {
        return 0;
    }

    /* Reset por software */
    BME280_Write8(BME280_REG_RESET, 0xB6);
    __delay_ms(100);

    /* Esperar a que copie calibracion desde NVM */
    timeout = 1000;
    while((BME280_Read8(BME280_REG_STATUS) & 0x01) && timeout)
    {
        __delay_ms(1);
        timeout--;
    }

    BME280_ReadCalibration();

    /*
     * Configuracion normal:
     * Humedad x1, temperatura x1, presion x1, modo normal.
     * Aunque uses LM35 para mostrar temperatura, el BME280 necesita su
     * temperatura interna para compensar humedad.
     */
    BME280_Write8(BME280_REG_CTRL_HUM, 0x01);    /* osrs_h = x1 */
    BME280_Write8(BME280_REG_CONFIG,   0x00);    /* standby 0.5 ms, filtro apagado */
    BME280_Write8(BME280_REG_CTRL_MEAS, 0x27);   /* temp x1, presion x1, normal */

    __delay_ms(10);

    bme280_ok = 1;
    return 1;
}

/* Lee presion, temperatura y humedad crudas. */
static void BME280_UpdateRaw(void)
{
    uint8_t data[8];

    BME280_ReadBytes(BME280_REG_PRESS_MSB, data, 8);

    adc_P = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | ((int32_t)data[2] >> 4);
    adc_T = ((int32_t)data[3] << 12) | ((int32_t)data[4] << 4) | ((int32_t)data[5] >> 4);
    adc_H = ((int32_t)data[6] << 8)  |  (int32_t)data[7];
}

/* Calcula temperatura y actualiza bme280_t_fine. */
static float BME280_CompensateTemperature(void)
{
    int32_t var1;
    int32_t var2;
    int32_t T;

    var1 = ((((adc_T >> 3) - ((int32_t)bme280_calib.dig_T1 << 1))) *
            ((int32_t)bme280_calib.dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((int32_t)bme280_calib.dig_T1)) *
              ((adc_T >> 4) - ((int32_t)bme280_calib.dig_T1))) >> 12) *
            ((int32_t)bme280_calib.dig_T3)) >> 14;

    bme280_t_fine = var1 + var2;
    T = (bme280_t_fine * 5 + 128) >> 8;

    return ((float)T) / 100.0f;
}

float BME280_ReadTemperature(void)
{
    if(!bme280_ok)
    {
        return -1000.0f;
    }

    BME280_UpdateRaw();
    return BME280_CompensateTemperature();
}

float BME280_ReadHumidity(void)
{
    int32_t v_x1_u32r;
    uint32_t hum_q1024;

    if(!bme280_ok)
    {
        return -1.0f;
    }

    /* Primero actualiza datos crudos y t_fine. */
    BME280_UpdateRaw();
    BME280_CompensateTemperature();

    v_x1_u32r = bme280_t_fine - ((int32_t)76800);

    v_x1_u32r = (((((adc_H << 14) - (((int32_t)bme280_calib.dig_H4) << 20) -
                    (((int32_t)bme280_calib.dig_H5) * v_x1_u32r)) +
                   ((int32_t)16384)) >> 15) *
                 (((((((v_x1_u32r * ((int32_t)bme280_calib.dig_H6)) >> 10) *
                      (((v_x1_u32r * ((int32_t)bme280_calib.dig_H3)) >> 11) +
                       ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
                    ((int32_t)bme280_calib.dig_H2) + 8192) >> 14));

    v_x1_u32r = v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                              ((int32_t)bme280_calib.dig_H1)) >> 4);

    if(v_x1_u32r < 0)
    {
        v_x1_u32r = 0;
    }

    if(v_x1_u32r > 419430400)
    {
        v_x1_u32r = 419430400;
    }

    hum_q1024 = (uint32_t)(v_x1_u32r >> 12);

    /* Resultado en porcentaje de humedad relativa. */
    return ((float)hum_q1024) / 1024.0f;
}

float BME280_ReadPressure_hPa(void)
{
    int64_t var1;
    int64_t var2;
    int64_t p;

    if(!bme280_ok)
    {
        return -1.0f;
    }

    BME280_UpdateRaw();
    BME280_CompensateTemperature();

    var1 = ((int64_t)bme280_t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)bme280_calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)bme280_calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)bme280_calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)bme280_calib.dig_P3) >> 8) +
           ((var1 * (int64_t)bme280_calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)bme280_calib.dig_P1) >> 33;

    if(var1 == 0)
    {
        return -1.0f;
    }

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)bme280_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)bme280_calib.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)bme280_calib.dig_P7) << 4);

    /* p queda en Pa * 256. Convertimos a hPa. */
    return ((float)p) / 25600.0f;
}
