/*
 * band.c
 *
 * Created: 27/05/2026 5:10:41 p. m.
 * Author : Institucional
 */ 

#include <avr/io.h>
#include "setup.h"


int main(void)
{
	//volatile uint16_t target_freq = 750;
	ADC_init();
	DDRB=255;
	/* PB5 (OC1A) como salida para señal hardware de timer */
	//DDRB  |= (1 << PB5);
	PORTB=0;

    //iniciar a velocidad normal (800Hz)
    timer1_ctc_init(800);

    /* Habilitar interrupciones globales */
    sei();

    while (1)
    {
		prom_FFT();
		working();
		_delay_ms(300);
        /*
         * Ejemplo: cambiar frecuencia desde aquí.
         * En una app real vendría de UART, ADC, botones, etc.
         *
         * timer1_set_freq(200);   → cambia a 200 Hz
         * timer1_set_freq(1000);  → cambia a 1000 Hz
         */
    }
}



