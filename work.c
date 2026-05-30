/*
 * work.c
 *
 * Created: 27/05/2026 5:13:28 p. m.
 *  Author: Institucional
 */ 
#include "setup.h"

ISR(TIMER1_COMPA_vect)
{
	/* Toggle PB0 (LED o señal de prueba) */
	PORTB ^= (1 << PB4);
}
//==================================================//
uint16_t mapping(int v_act){
	int mapped_val=0;
	mapped_val=F_MIN_HZ_VEL+(v_act-F_MIN_HZ_SND)*((F_MAX_HZ_VEL-F_MIN_HZ_VEL)/(F_MAX_HZ_SND-F_MIN_HZ_SND));
	return mapped_val;
}
//==================================================//
void timer1_ctc_init(uint16_t f_target){
	    /* Clamp al rango permitido */
	mapping(f_target);
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
	mapping(f_target);
	if (f_target < F_MIN_HZ_VEL) f_target = F_MIN_HZ_VEL;
	if (f_target > F_MAX_HZ_VEL) f_target = F_MAX_HZ_VEL;

	uint16_t ocr_val = OCR_FROM_FREQ(f_target);

	cli();          /* deshabilitar interrupciones */
	OCR1A = ocr_val;
	sei();          /* rehabilitar interrupciones  */
}