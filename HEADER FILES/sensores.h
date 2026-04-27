/* 
 * File:   sensores.h
 * Author: Castel JM
 *
 * Created on 24 de abril de 2026, 02:15 PM
 */

#include <xc.h>

#define MQ135_DO_PORT   PORTBbits.RB2
#define MQ135_DO_TRIS   TRISBbits.TRISB2
#define RELE_PIN        LATBbits.LATB3
#define RELE_TRIS       TRISBbits.TRISB3
#define ON         		1   // Cambia a 0 si tu modulo es activo en bajo
#define OFF        		0

// --- Umbrales para ventilador
#define HUM_BAJA    10      // % - por debajo apaga ventilador
#define HUM_MEDIA   30      // % - por debajo velocidad media


// --- Umbrales de histéresis (°C) ---
#define TEMP_LOW        26.0f   // Por debajo de esto: enciende calentador
#define TEMP_HIGH       28.0f   // Por encima de esto: apaga calentador


float LM35_ObtenerTemp(void);
float LDR_ObtenerVoltaje(void);
unsigned int LDR_ObtenerPorcentaje(void);
void MQ135_Init(void);
unsigned char MQ135_GasDetectado(void);
float leer_humedad(void);

void PWM_Init(void);
void PWM_SetDuty(unsigned char porcentaje); 
