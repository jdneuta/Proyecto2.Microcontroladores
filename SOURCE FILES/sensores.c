/**
 * @file    sensores.c
 * @brief   Implementacion de lectura de sensores y control PWM/DHT11.
 * @version 1.0
 *
 * @details
 * Contiene funciones para leer MQ135, LDR, BME280, controlar PWM y obtener
 * datos del sensor DHT11 mediante protocolo de un solo hilo.
 */

#include "config.h"
#include "sensores.h"
#include "adc.h"
#include "i2c.h"
#include "bme280_lib.h"

/**
 * @brief Lee el sensor de gas MQ135.
 *
 * @return Valor ADC correspondiente al canal AN0.
 */
unsigned int MQ135_Leer(void) {
    return ADC_Read(0);
}

/**
 * @brief Lee el sensor de luz LDR y promedia varias muestras.
 *
 * @details
 * Toma 16 muestras del canal AN1 y calcula el promedio para reducir ruido.
 *
 * @return Valor ADC promedio de la LDR.
 */
unsigned int LDR_Leer(void) {
    unsigned char i;
    unsigned long suma = 0;

    /** Acumula 16 conversiones ADC consecutivas. */
    for(i = 0; i < 16; i++)
    {
        suma += ADC_Read(1);
    }

    /** Retorna el promedio entero de las muestras. */
    return (unsigned int)(suma / 16UL);
}

/**
 * @brief Lee la humedad relativa desde el sensor BME280.
 *
 * @return Humedad relativa en porcentaje.
 */
float leer_humedad(void) {
    return BME280_ReadHumidity();
}

/**
 * @brief Inicializa el modulo PWM en CCP1.
 *
 * @details
 * Configura RC2 como salida, Timer2 como base de tiempo y CCP1 en modo PWM.
 */
void PWM_Init(void)
{
    /** Configura RC2/CCP1 como salida PWM. */
    TRISCbits.TRISC2 = 0;

    /** Define el periodo PWM. */
    PR2   = 129;

    /** Enciende Timer2 con prescaler 1:16. */
    T2CON = 0b00000111;

    /** Configura CCP1 en modo PWM. */
    CCP1CON = 0b00001100;

    /** Inicia el ciclo de trabajo en 0 %. */
    CCPR1L  = 0;
}

/**
 * @brief Establece el ciclo de trabajo PWM.
 *
 * @param porcentaje Ciclo de trabajo deseado entre 0 y 100 %.
 */
void PWM_SetDuty(unsigned char porcentaje)
{
    unsigned int duty;

    /** Limita el porcentaje maximo a 100 %. */
    if(porcentaje > 100) porcentaje = 100;

    /** Calcula los ticks del PWM: porcentaje * 4 * (PR2 + 1) / 100. */
    duty = ((unsigned int)porcentaje * 520U) / 100U;

    /** Coloca los dos bits menos significativos en CCP1CON<5:4>. */
    CCP1CONbits.DC1B = (unsigned char)(duty & 0x03);

    /** Coloca los bits mas significativos del duty en CCPR1L. */
    CCPR1L           = (unsigned char)(duty >> 2);
}

/**
 * @brief Envia la condicion de inicio al sensor DHT11.
 *
 * @details
 * El microcontrolador pone el pin de datos en bajo durante al menos 18 ms,
 * luego libera la linea para que el DHT11 pueda responder.
 */
void DHT11_Start(void)
{
    DHT11_PIN_DIR = 0;
    DHT11_PIN_LAT = 0;
    __delay_ms(20);
    DHT11_PIN_LAT = 1;
    __delay_us(30);
    DHT11_PIN_DIR = 1;
}

/**
 * @brief Espera la respuesta de confirmacion del DHT11.
 *
 * @details
 * La funcion espera las transiciones de la linea de datos que indican que el
 * sensor recibio la senal de inicio y comenzara a enviar informacion.
 */
void DHT11_Response(void) {
    while(DHT11_PIN_PORT == 1);
    while(DHT11_PIN_PORT == 0);
    while(DHT11_PIN_PORT == 1);
}

/**
 * @brief Lee un byte del flujo de datos del DHT11.
 *
 * @details
 * Cada bit se determina midiendo el estado de la linea despues de una espera
 * de 30 us. Si la linea sigue alta, el bit se interpreta como 1; si no, como 0.
 *
 * @return Byte recibido desde el DHT11.
 */
int DHT11_Read_Byte(void) {
    int i, data = 0;

    /** Lee los 8 bits del byte, del mas significativo al menos significativo. */
    for(i = 0; i < 8; i++){
        while((DHT11_PIN_PORT) == 0);
        __delay_us(30);

        if((DHT11_PIN_PORT) == 1){
            data = ((data << 1) | 1);
        }
        else{
            data = (data << 1);
        }

        while((DHT11_PIN_PORT) == 1);
    }

    return data;
}

/**
 * @brief Lee humedad y temperatura desde el DHT11.
 *
 * @param tem Puntero donde se almacena la temperatura en grados Celsius.
 * @param hum Puntero donde se almacena la humedad relativa en porcentaje.
 * @return 1 si el checksum es correcto; 0 si el checksum falla.
 */
short DHT11_Read_Data(float *tem, float *hum) {
    unsigned int checksum = 0;
    unsigned int info[5];

    /** Inicia la comunicacion y espera la respuesta del sensor. */
    DHT11_Start();
    DHT11_Response();

    /** Lee los 5 bytes: humedad, temperatura y checksum. */
    info[0] = (unsigned int)DHT11_Read_Byte();
    info[1] = (unsigned int)DHT11_Read_Byte();
    info[2] = (unsigned int)DHT11_Read_Byte();
    info[3] = (unsigned int)DHT11_Read_Byte();
    info[4] = (unsigned int)DHT11_Read_Byte();

    /** Convierte los datos recibidos a valores flotantes con una cifra decimal. */
    *hum = (float)(DHT11_Join_Data(info[0], info[1])) / 10.0f;
    *tem = (float)(DHT11_Join_Data(info[2], info[3])) / 10.0f;

    /** Calcula checksum de los primeros cuatro bytes. */
    checksum = info[0] + info[1] + info[2] + info[3];

    /** Valida si el checksum calculado coincide con el enviado por el sensor. */
    if(checksum == info[4]) {
        return 1;
    }
    else {
        return 0;
    }
}

/**
 * @brief Combina dos partes numericas en un solo valor.
 *
 * @details
 * Multiplica la parte alta por la potencia de 10 necesaria y suma la parte
 * baja, permitiendo reconstruir datos separados del DHT11.
 *
 * @param h Parte alta o entera.
 * @param l Parte baja o decimal.
 * @return Valor combinado.
 */
unsigned DHT11_Join_Data(unsigned h, unsigned l) {
    unsigned pow = 10;

    /** Ajusta la potencia segun la cantidad de digitos de la parte baja. */
    while(l >= pow)
        pow *= 10;

    return h * pow + l;
}
