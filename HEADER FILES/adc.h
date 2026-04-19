/* 
 * File:   adc.h
 * Author: MARIA DE LOS ANGELES
 *
 * Created on 18 de abril de 2026, 22:44
 */
#ifndef ADC_H
#define ADC_H

#include <xc.h>

#define AN0_ANALOG          0x0E
#define AN0_AN1_ANALOG      0x0D
#define AN0_TO_AN2_ANALOG   0x0C
#define AN0_TO_AN3_ANALOG   0x0B
#define AN0_TO_AN4_ANALOG   0x0A
#define AN0_TO_AN5_ANALOG   0x09
#define AN0_TO_AN6_ANALOG   0x08
#define AN0_TO_AN7_ANALOG   0x07
#define AN0_TO_AN8_ANALOG   0x06
#define AN0_TO_AN9_ANALOG   0x05
#define AN0_TO_AN10_ANALOG  0x04
#define AN0_TO_AN11_ANALOG  0x03
#define ALL_ANALOG          0x00

void ADC_Init(unsigned char p_ang);
unsigned int ADC_Read(unsigned char ch);
float LM35_ObtenerTemp(void);
float LDR_ObtenerVoltaje(void);
unsigned int LDR_ObtenerPorcentaje(void);

#endif