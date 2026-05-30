/*
 * band.c
 *
 * Created: 27/05/2026 5:10:41 p. m.
 * Author : Institucional
 */ 

#include <avr/io.h>
#include "setup.h"

volatile uint16_t target_freq = 750;

int main(void)
{
    /* PB0 como salida (LED de prueba) */
    DDRB  |= (1 << PB4);
    /* PB5 (OC1A) como salida para señal hardware de timer */
    DDRB  |= (1 << PB5);

    /* Iniciar CTC a 500 Hz */
    timer1_ctc_init(target_freq);

    /* Habilitar interrupciones globales */
    sei();

    while (1)
    {
        /*
         * Ejemplo: cambiar frecuencia desde aquí.
         * En una app real vendría de UART, ADC, botones, etc.
         *
         * timer1_set_freq(200);   → cambia a 200 Hz
         * timer1_set_freq(1000);  → cambia a 1000 Hz
         */
    }
}



