/**
 * @file    sensores.h
 * @author  MARIA DE LOS ANGELES
 * @brief   Definiciones de pines, umbrales y prototipos para sensores y actuadores.
 * @version 1.0
 * @date    2026-04-24
 *
 * @details
 * Este archivo centraliza las constantes usadas por sensores MQ135, LDR,
 * DHT11 y BME280, ademas de salidas como rele, buzzer, LEDs y PWM.
 */

#ifndef SENSORES_H
#define SENSORES_H

#include <xc.h>

/** @brief Estado logico encendido para salidas digitales. */
#define ON                 1

/** @brief Estado logico apagado para salidas digitales. */
#define OFF                0

/** @brief Canal ADC usado por el sensor de gas MQ135. */
#define MQ135_AN_CH        0

/** @brief Pin de salida que controla el rele. */
#define RELE_PIN           LATBbits.LATB3

/** @brief Registro TRIS asociado al pin del rele. */
#define RELE_TRIS          TRISBbits.TRISB3

/** @brief Pin de salida que controla la alarma. */
#define ALARMA_PIN         LATBbits.LATB4

/** @brief Registro TRIS asociado al pin de la alarma. */
#define ALARMA_TRIS        TRISBbits.TRISB4

/** @brief Pin de salida del LED 1. */
#define LED_PIN            LATAbits.LATA3

/** @brief Registro TRIS asociado al LED 1. */
#define LED_TRIS           TRISAbits.TRISA3

/** @brief Pin de salida del LED 2. */
#define LED2_PIN           LATAbits.LATA4

/** @brief Registro TRIS asociado al LED 2. */
#define LED2_TRIS          TRISAbits.TRISA4

/** @brief Pin de salida del LED 3. */
#define LED3_PIN           LATAbits.LATA5

/** @brief Registro TRIS asociado al LED 3. */
#define LED3_TRIS          TRISAbits.TRISA5

/** @brief Pin de salida del LED 4. */
#define LED4_PIN           LATEbits.LATE0

/** @brief Registro TRIS asociado al LED 4. */
#define LED4_TRIS          TRISEbits.TRISE0

/** @brief Pin de salida del LED 5. */
#define LED5_PIN           LATEbits.LATE1

/** @brief Registro TRIS asociado al LED 5. */
#define LED5_TRIS          TRISEbits.TRISE1

/** @brief Umbral ADC para considerar luz baja. */
#define LUZ_BAJA           15

/** @brief Umbral ADC para considerar luz media. */
#define LUZ_MEDIA          65

/** @brief Umbral ADC para considerar luz moderada. */
#define LUZ_MODERADA       150

/** @brief Umbral ADC para considerar luz alta. */
#define LUZ_ALTA           180

/** @brief Humedad por debajo de la cual se apaga el ventilador. */
#define HUM_APAGADO        69

/** @brief Humedad desde la cual se activa el ventilador. */
#define HUM_ALTA           70

/** @brief Temperatura baja para activar el calentador, en grados Celsius. */
#define TEMP_LOW           28.0f

/** @brief Temperatura alta para apagar el calentador, en grados Celsius. */
#define TEMP_HIGH          30.0f

/** @brief Lectura digital del pin de datos del DHT11. */
#define DHT11_PIN_PORT     PORTBbits.RB2

/** @brief Registro LAT del pin de datos del DHT11. */
#define DHT11_PIN_LAT      LATBbits.LATB2

/** @brief Registro TRIS del pin de datos del DHT11. */
#define DHT11_PIN_DIR      TRISBbits.TRISB2

/**
 * @brief Obtiene temperatura desde un sensor LM35.
 *
 * @return Temperatura en grados Celsius.
 *
 * @note El prototipo se conserva por compatibilidad; no aparece implementado
 * en los archivos entregados.
 */
float LM35_ObtenerTemp(void);

/**
 * @brief Lee el sensor LDR.
 * @return Valor ADC promedio asociado al nivel de luz.
 */
unsigned int LDR_Leer(void);

/**
 * @brief Lee el sensor de gas MQ135.
 * @return Valor ADC asociado al nivel de gas/contaminantes.
 */
unsigned int MQ135_Leer(void);

/**
 * @brief Lee la humedad relativa desde el BME280.
 * @return Humedad relativa en porcentaje.
 */
float leer_humedad(void);

/** @brief Inicializa el modulo PWM usado para controlar el ventilador. */
void PWM_Init(void);

/**
 * @brief Ajusta el ciclo de trabajo del PWM.
 * @param porcentaje Ciclo de trabajo deseado entre 0 y 100 %.
 */
void PWM_SetDuty(unsigned char porcentaje);

/** @brief Envia la senal de inicio al sensor DHT11. */
void DHT11_Start(void);

/** @brief Espera y valida la respuesta inicial del DHT11. */
void DHT11_Response(void);

/**
 * @brief Lee un byte enviado por el DHT11.
 * @return Byte recibido desde el sensor.
 */
int DHT11_Read_Byte(void);

/**
 * @brief Lee temperatura y humedad desde el DHT11.
 *
 * @param temp Puntero donde se almacena la temperatura en grados Celsius.
 * @param hum Puntero donde se almacena la humedad relativa en porcentaje.
 * @return 1 si el checksum es correcto; 0 si la lectura no es valida.
 */
short DHT11_Read_Data(float *temp, float *hum);

/**
 * @brief Une la parte entera y decimal de una medicion del DHT11.
 *
 * @param h Parte alta o entera del dato.
 * @param l Parte baja o decimal del dato.
 * @return Valor combinado.
 */
unsigned DHT11_Join_Data(unsigned h, unsigned l);

#endif
