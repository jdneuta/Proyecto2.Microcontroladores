#include "adc.h"
#include "config.h"

void ADC_Init(unsigned char p_ang) {
    ADCON1 = p_ang;
    ADCON0 = 0x01;   // Canal AN0 seleccionado, ADC encendido
    ADCON2 = 0xA2;   // Justificado derecha, 8TAD, Fosc/32
}

unsigned int ADC_Read(unsigned char ch) {
    if(ch > 12) {
        return 0;
    }

    ADCON0 &= 0xC3;          // Limpia bits CHS3:CHS0
    ADCON0 |= (ch << 2);     // Selecciona canal
    ADCON0bits.ADON = 1;

    __delay_us(20);          // Tiempo de adquisicion

    ADCON0bits.GO_DONE = 1;
    while(ADCON0bits.GO_DONE == 1);

    return ((unsigned int)ADRESH << 8) | ADRESL;
}

