/* 
 * File:   sensores.h
 * Author: MARIA DE LOS ANGELES
 *
 * Created on 24 de abril de 2026, 02:15 PM
 */

#include <xc.h>

#define ON         		1   // Cambia a 0 si tu modulo es activo en bajo
#define OFF        		0

//Pin para MQ135
#define MQ135_DO_PORT   PORTBbits.RB2
#define MQ135_DO_TRIS   TRISBbits.TRISB2

//Pin Relay
#define RELE_PIN        LATBbits.LATB3
#define RELE_TRIS       TRISBbits.TRISB3

// --- Pines de calidad de aire

#define BUZZER_PIN      LATBbits.LATB4
#define BUZZER_TRIS     TRISBbits.TRISB4

//Pines Led
#define LED_PIN         LATBbits.LATB5
#define LED_TRIS        TRISBbits.TRISB5

#define LED2_PIN        LATBbits.LATB6
#define LED2_TRIS       TRISBbits.TRISB6

// --- Umbral para la luz
#define LUZ_UMBRAL      45 

// --- Umbrales para ventilador
#define VEN_PIN         LATBbits.LATB7
#define VEN_TRIS        TRISBbits.TRISB7
#define HUM_APAGADO     30
#define HUM_BAJA        50      
#define HUM_ALTA        60    

// --- Umbrales de histéresis (°C) ---
#define TEMP_LOW        26.0f   // Por debajo de esto: enciende calentador
#define TEMP_HIGH       28.0f   // Por encima de esto: apaga calentador

// --- Pines de calidad de aire
#define BUZZER_PIN      LATBbits.LATB4
#define BUZZER_TRIS     TRISBbits.TRISB4



float LM35_ObtenerTemp(void);

float LDR_ObtenerVoltaje(void);
unsigned int LDR_ObtenerPorcentaje(void);

void MQ135_Init(void);
unsigned char MQ135_GasDetectado(void);

float leer_humedad(void);

void PWM_Init(void);
void PWM_SetDuty(unsigned char porcentaje); 
