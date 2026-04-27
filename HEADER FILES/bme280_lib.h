#ifndef BME280_LIB_H
#define BME280_LIB_H

#include <xc.h>
#include <stdint.h>

/*
 * Libreria BME280 para MPLAB X + XC8
 * Adaptada para usar las funciones I2C del proyecto:
 * I2C_Start(), I2C_Restart(), I2C_Write(), I2C_Read(), I2C_Ack(), I2C_Nack(), I2C_Stop().
 *
 * Direccion I2C de 7 bits:
 * - Muchos modulos usan 0x76
 * - Otros usan 0x77
 * Si no detecta el sensor, cambia BME280_ADDRESS a 0x77.
 */
#define BME280_ADDRESS  0x76

/* Inicializa el BME280. Devuelve 1 si encontro el sensor, 0 si fallo. */
uint8_t BME280_Init(void);

/* Alias por si prefieres este nombre. */
#define BME280_Begin() BME280_Init()

/* Lecturas completas. */
float BME280_ReadTemperature(void);   /* grados Celsius */
float BME280_ReadHumidity(void);      /* % humedad relativa */
float BME280_ReadPressure_hPa(void);  /* hPa */

#endif
