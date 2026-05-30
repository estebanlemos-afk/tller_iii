/*
 * setup.h
 *
 * Created: 27/05/2026 5:13:13 p. m.
 *  Author: Institucional
 */ 


#ifndef SETUP_H_
#define SETUP_H_

/*
 * Timer1 en modo CTC - ATmega2560
 * f_CPU = 16 MHz
 * Rango de frecuencias: 127 Hz a 1273 Hz
 *
 * Fórmula:  f_CTC = f_CPU / (2 * N * (1 + OCR1A))
 * Despejando OCR1A:
 *           OCR1A = (f_CPU / (2 * N * f_target)) - 1
 *
 * Con N = 64:
 *   f = 127 Hz  → OCR1A = (16000000 / (2*64*127)) - 1 = 984
 *   f = 1273 Hz → OCR1A = (16000000 / (2*64*1273)) - 1 = 97
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define F_CPU			16000000UL
#define PRESCALER		64
#define F_MIN_HZ_VEL	127
#define F_MAX_HZ_VEL    1273
#define F_MIN_HZ_SND	700
#define F_MAX_HZ_SND	900

/* OCR1A para frecuencia objetivo con N=64 */
#define OCR_FROM_FREQ(f) ((uint16_t)((F_CPU / (2 * PRESCALER * (f))) - 1))

void timer1_ctc_init(uint16_t f_target);
void timer1_set_freq(uint16_t f_target);
uint16_t mapping(int v_act);




#endif /* SETUP_H_ */