/*
 * fft_t_iii.c
 *
 * Created: 17/02/2026 11:02:23 a. m.
 * Author : Institucional
 */ 
#include "setup.h"

int main(void)
{
	ADC_init();
	uart_init(MYUBRR);
	DDRB = 255;
	PORTB=0;
	while(1)
	{
		PORTB|=(1<<PB0);
		capture_signal(0);
		ventana_hammin();
		calc_FFT();
		envia_info();
		_delay_ms(300);
		PORTB &=~(1<<PB0);
		_delay_ms(300);
	}
}

