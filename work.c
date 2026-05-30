/*
 * work.c
 *
 * Created: 27/05/2026 5:13:28 p. m.
 *  Author: Institucional
 */ 
#include "setup.h"
//FFT
void ADC_init() {
	DIDR0=0x00;
	DIDR2=0x00;
	ADMUX=0x40;
	ADCSRA=0x84;
	ADCSRB=0x00;
}
//==================================================//
uint16_t ADC_read(uint8_t adc_input)
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
//==================================================//
void capture_signal(unsigned char ch) {
	for (unsigned int i = 0; i < N; i++) {
		REX[i] = ADC_read(ch);
		_delay_us(TS);
	}
}
//==================================================//
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
//==================================================//
void ventana_hammin(void) {
	for (uint16_t i = 0; i < N; i++) {
		float w = 0.54- 0.46 * cosf((2.0 * (float)M_PI * i) / (N- 1));
		REX[i] *= w;
	}
}
//==================================================//
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
//==================================================//
int idx=0;
float freq=0,mag=0;
void prom_FFT(){
	float aux_freq=0,aux_mag=0;
	for (int i = 0; i < 5; i++) {
		capture_signal(1);   // recaptura señal fresca
		ventana_hammin();    // aplica ventana
		calc_FFT();          // calcula FFT
		idx = max_indx();
		aux_mag  += Mag[idx];
		aux_freq += (idx * (float)FSAMPLE) / (float)N;
	}
	freq = aux_freq / 5;
	mag  = aux_mag  / 5;
}
//==================================================//

//band
ISR(TIMER1_COMPA_vect)
{
	/* Toggle PB0 (LED o señal de prueba) */
	//PORTB ^= (1 << PB4); */no hace nada epicamente*/
}
//==================================================//
uint16_t mapping(int v_act){
	int mapped_val=0;
	mapped_val=F_MIN_HZ_VEL+(v_act-F_MIN_HZ_SND)*((float)(F_MAX_HZ_VEL-F_MIN_HZ_VEL)/(float)(F_MAX_HZ_SND-F_MIN_HZ_SND));
	return mapped_val;
}
//==================================================//
void timer1_ctc_init(uint16_t f_target){
	    /* Clamp al rango permitido */
    if (f_target < F_MIN_HZ_VEL) f_target = F_MIN_HZ_VEL;
    if (f_target > F_MAX_HZ_VEL) f_target = F_MAX_HZ_VEL;

    uint16_t ocr_val = OCR_FROM_FREQ(f_target);

    /* Detener timer antes de configurar */
    TCCR1B = 0;

    /* Modo CTC: WGM12=1 (bits WGM13:0 = 0b0100)
     * COM1A0=1: toggle OC1A en comparación (pin PB5)
     * COM1A1=0: modo toggle normal */
    TCCR1A = (1 << COM1A0);                 /* toggle OC1A */
    TCCR1B = (1 << WGM12);                  /* CTC con TOP=OCR1A */

    /* Registro de comparación */
    OCR1A  = ocr_val;

    /* Limpiar contador */
    TCNT1  = 0;

    /* Habilitar interrupción por comparación A */
    TIMSK1 = (1 << OCIE1A);

    /* Aplicar prescaler N=64 y arrancar timer
     * CS12:CS10 = 011 → N=64 */
    TCCR1B |= (1 << CS11) | (1 << CS10);

}
//==================================================//
void timer1_set_freq(uint16_t f_target){
	if (f_target < F_MIN_HZ_VEL) f_target = F_MIN_HZ_VEL;
	if (f_target > F_MAX_HZ_VEL) f_target = F_MAX_HZ_VEL;

	uint16_t ocr_val = OCR_FROM_FREQ(f_target);

	cli();          /* deshabilitar interrupciones */
	OCR1A = ocr_val;
	sei();          /* rehabilitar interrupciones  */
}
//==================================================//
void working(){
	if (freq > F_MIN_HZ_SND && freq < F_MAX_HZ_SND) {
		timer1_set_freq(mapping((int)freq)); 
		} else {
		timer1_set_freq(800);
	}
}