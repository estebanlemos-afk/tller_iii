#define F_CPU 16000000UL
#include <avr/io.h>

void ADC_init() {
	// Referencia AVcc (5V), canal ADC0
	ADMUX = (1 << REFS0);

	// Habilitar ADC, prescaler 128 (125 kHz con 16 MHz)
	ADCSRA = (1 << ADEN) |
	(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t ADC_read(uint8_t channel) {
	// Seleccionar canal (ADC0 = 0)
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

	// Iniciar conversión
	ADCSRA |= (1 << ADSC);

	// Esperar a que termine
	while (ADCSRA & (1 << ADSC));

	return ADC;
}

int main(void) {
	ADC_init();

	// Configurar PB7 como salida (LED)
	DDRB |= (1 << PB7);

	while (1) {
		uint16_t adc_value = ADC_read(0);

		// Convertir a voltaje (suponiendo Vref = 5V)
		float voltage = (adc_value * 5.0) / 1023.0;

		// Comparar con umbral de 1.7V
		if (voltage > 1.8) {
			PORTB |= (1 << PB7);  // Encender LED
			} else {
			PORTB &= ~(1 << PB7); // Apagar LED
		}
	}
}
