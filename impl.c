/*
 * impl.c
 *
 * Created: 17/02/2026 11:31:12 a. m.
 *  Author: Institucional
*/ 
#include "setup.h"

void uart_init(unsigned int ubrr) {
	UBRR0H =(unsigned char)(ubrr >>8);
	UBRR0L =(unsigned char)ubrr;
	UCSR0B =(1 <<RXEN0) |(1 << TXEN0);
	UCSR0C =(1 <<UCSZ01)| (1<< UCSZ00);
}
//88888888888888888888888888888888888888888888888888888888
void ADC_init() {
	DIDR0=0x00;
	DIDR2=0x00;
	ADMUX=0x40;
	ADCSRA=0x84;
	ADCSRB=0x00;
}
//88888888888888888888888888888888888888888888888888888888
unsigned int ADC_read(unsigned char adc_input)
{
	ADMUX=(adc_input & 0x1F) | 0X40;
	if(adc_input& 0x20) ADCSRB|=(1<<MUX5);
	else ADCSRB&=~(1<<MUX5);
	_delay_us(1);
	ADCSRA|=(1<<ADSC);
	while ((ADCSRA & (1<<ADIF))==0);
	ADCSRA|=(1<<ADIF);
	return ADCW;
}
//88888888888888888888888888888888888888888888888888888888888888888888
void uart_transmit(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}
//88888888888888888888888888888888888888888888888888888888888888888888
void uart_print(const char *str) {
	while (*str) {
		uart_transmit(*str++);
	}
}
//88888888888888888888888888888888888888888888888888888888888888888888
void capture_signal(unsigned char canal) {
	for (unsigned int i = 0; i < N; i++) {
		REX[i] = ADC_read(canal);
		_delay_us(TS);
	}
}
//88888888888888888888888888888888888888888888888888888888888888888888
void envia_info(){
	char buffer[35];
	char buffer1[50]; // Un poco más grande para que quepa todo el texto
	uart_print("FFT MAGN_FREC:\r\n\r\n");
	
	// Bucle original para imprimir todas las frecuencias
	for(int i = 0; i < N_out; i++){
		float frequency = (i * (float)FSAMPLE) / (float)N;
		float mag = Mag[i];

		if(mag < 0) mag = 0.0f;

		int freq_int = (int)frequency;
		int freq_dec = (int)((frequency - freq_int) * 10);
		if(freq_dec < 0) freq_dec = -freq_dec;

		int mag_int  = (int)mag;
		int mag_dec  = (int)((mag - mag_int) * 10);
		if(mag_dec < 0) mag_dec = -mag_dec;

		sprintf(buffer, "F:%d.%d Hz, M:%d.%d\r\n", freq_int, freq_dec, mag_int, mag_dec);
		uart_print(buffer);
	}
	
	// --- NUEVO: Calcular frecuencia y magnitud máxima ---
	
	// 1. Obtenemos la posición del máximo
	int max_idx = max_indx();
	
	// 2. Leemos la magnitud en esa posición
	float max_mag = Mag[max_idx];
	
	// 3. Calculamos la frecuencia correspondiente a esa posición
	float max_freq = (max_idx * (float)FSAMPLE) / (float)N;
	
	// 4. Preparamos los valores para imprimirlos (enteros y decimales)
	int mag_int = (int)max_mag;
	int mag_dec = (int)((max_mag - mag_int) * 10);
	if (mag_dec < 0) mag_dec = -mag_dec;
	
	int freq_int = (int)max_freq;
	int freq_dec = (int)((max_freq - freq_int) * 10);
	if (freq_dec < 0) freq_dec = -freq_dec;
	
	// 5. Imprimimos el resultado final
	sprintf(buffer1, "\r\n--> MAX FREC: %d.%d Hz (Mag: %d.%d)\r\n", freq_int, freq_dec, mag_int, mag_dec);
	uart_print(buffer1);
	
	float freq_min = 700.0;
	float freq_max = 900.0;
	
	if (max_freq > freq_min && max_freq < freq_max) {
		PORTB |= (1 << PB1);  // Enciende el LED en PB1 (Pin 9 en Arduino)
		} else {
		PORTB &= ~(1 << PB1); // Apaga el LED en PB1
	}
}
//88888888888888888888888888888888888888888888888888888888888888888888
void calc_FFT()
{
	NM1= N-1;
	ND2= N/2;
	M =ceil(log(N)/log(2));
	J =ND2;
	for(I=1; I<=N-2; I++)
	{
		if(I>=J)
		{
			goto L1;
		}
		TR=REX[J];
		TI=IMX[J];
		REX[J] = REX[I];
		IMX[J] = IMX[I];
		REX[I] = TR;
		IMX[I] =TI;
		L1: K= ND2;
		L2: if(K>J)
		{
			goto L3;
		}
		J =J-K;
		K =K/2;
		goto L2;
		L3: J = J+K;
	}
		for(L=1; L<=M;L++)
		{
			LE=ceil(pow(2,L));//busca entero mas cercano
			LE2=LE/2;
			UR=1;
			UI=0;
			SR=cos(M_PI/LE2);
			SI=-sin(M_PI/LE2);
			for(J=1; J<=LE2; J++)
			{
				JM1=J-1;
				for(I=JM1;I<=NM1; I=I+LE)
				{
					IP=I+LE2;
					TR=REX[IP]*UR-IMX[IP]*UI;
					TI=REX[IP]*UI+IMX[IP]*UR;
					REX[IP] =REX[I]-TR;
					IMX[IP] =IMX[I]-TI;
					REX[I] =REX[I]+TR;
					IMX[I] =IMX[I]+TI;
				}
				TR=UR;
				UR=TR*SR-UI*SI;
				UI=TR*SI+UI*SR;
			}
		}
		REX[0]=REX[0]/2;
		IMX[0]=IMX[0]/2;
		
		for(L=0; L<=N-1; L++)
		{
			REX[L] = REX[L]/ND2;
			IMX[L] = IMX[L]/ND2;
		}
		for(L=0; L<=((N/2)-1);L++)
		{
			Mag[L] = sqrt(REX[L]*REX[L]+ IMX[L]*IMX[L]);
		}
	}
	
void ventana_hammin(void) {
	for (uint16_t i = 0; i < N; i++) {
		float w = 0.54- 0.46 * cosf((2.0 * (float)M_PI * i) / (N- 1));
		REX[i] *= w;
	}
}

int max_indx()
{
	float max_val = -1.0;
	int max_idx = 1; // Asumimos por defecto el índice 1 por si acaso
	
	// Empezamos desde i = 1 para saltarnos Mag[0] (0 Hz / DC)
	for (int i = 1; i < N_out; i++) {
		if (Mag[i] > max_val) {
			max_val = Mag[i];
			max_idx = i; // Guardamos en qué posición ocurrió el máximo
		}
	}
	return max_idx;
}