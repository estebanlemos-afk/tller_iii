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
	ADC_init();
	uart_init(MYUBRR);
	init_pwm();
	DDRB=255;
	PORTB=0;

    //iniciar a velocidad normal (500Hz)
    timer1_ctc_init(500);
    sei();

    while (1)
    {
		prom_FFT();
		working();
		envia_info();
		_delay_ms(300);
		OCR3A=POS_IZQUIERDA;
		_delay_ms(1500);
		OCR3A=POS_DERECHA;
		_delay_ms(1500);
		OCR3A=POS_CENTRO;
    }
}



