#define F_CPU 16000000UL
#include <avr/io.h>

void ADC_init() {
	ADMUX = (1 << REFS0);

	ADCSRA = (1 << ADEN) |
	(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t ADC_read(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));

	return ADC;
}

int main(void) {
	ADC_init();

	DDRB |= (1 << PB7)|(1<<PB1)|(1<<PB0);
	PORTB &= ~(1<<PB7);
	while (1) {
		
		//metal detec
		
		uint16_t adc_value = ADC_read(0);
		float voltage = (adc_value * 5.0) / 1023.0;	// Convertir a voltaje (vref=+5)

		if (voltage > 2.05) {
			PORTB |= (1 << PB0);
			} else {
			PORTB &= ~(1 << PB0);
		
		//presc detec
			
		uint16_t adc_value = ADC_read(1);
		float voltage = (adc_value * 5.0) / 1023.0;
		
		if(voltage<1.0){
			PORTB|=(1<<PB1);
			} else {
			PORTB &= ~(1 << PB1);
			}
		}
	}
}
