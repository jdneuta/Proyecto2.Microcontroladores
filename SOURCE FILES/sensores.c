#include "config.h"
#include "sensores.h"
#include "adc.h"
#include "i2c.h"
#include "bme280_lib.h"

void MQ135_Init(void)
{
    MQ135_DO_TRIS = 1;
}

unsigned char MQ135_GasDetectado(void){
    
    return (MQ135_DO_PORT == 0) ? 1 : 0;
}

float LM35_ObtenerTemp(void){
    
    unsigned char i;
    unsigned long suma = 0;
    unsigned int adc;
    float voltaje;
    float temperatura;

    for(i = 0; i < 16; i++)
    {
        suma += ADC_Read(0);
    }

    adc = (unsigned int)(suma / 16UL);

    voltaje = (adc * 5.0f) / 1023.0f;
    temperatura = (voltaje / 4.7f) * 100.0f;

    return temperatura;
}

float LDR_ObtenerVoltaje(void){
    
    unsigned char i;
    unsigned long suma = 0;
    unsigned int adc;

    for(i = 0; i < 16; i++)
    {
        suma += ADC_Read(1);
    }

    adc = (unsigned int)(suma / 16UL);
    return (adc * 5.0f) / 1023.0f;
}

unsigned int LDR_ObtenerPorcentaje(void){
    
    unsigned char i;
    unsigned long suma = 0;
    unsigned int adc;

    for(i = 0; i < 16; i++)
    {
        suma += ADC_Read(1);
    }

    adc = (unsigned int)(suma / 16UL);
    return (adc * 100U) / 1023U;
}

float leer_humedad(void) {
    
    return BME280_ReadHumidity();
}

void PWM_Init(void)
{
    TRISCbits.TRISC2 = 0;       // RC2 como salida
    PR2   = 129;                // Periodo PWM
    T2CON = 0b00000111;         // TMR2 ON, Prescaler 1:16
    CCP1CON = 0b00001100;       // Modo PWM
    CCPR1L  = 0;                // Duty cycle inicial = 0
}

void PWM_SetDuty(unsigned char porcentaje)
{
    unsigned int duty;

    if(porcentaje > 100) porcentaje = 100;

    // duty_ticks = porcentaje * (4 * (PR2+1)) / 100
    duty = ((unsigned int)porcentaje * 520U) / 100U;

    // Los 2 bits menos significativos van en CCP1CON<5:4>
    CCP1CONbits.DC1B = (unsigned char)(duty & 0x03);
    CCPR1L           = (unsigned char)(duty >> 2);
}
