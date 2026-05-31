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

#define F_CPU		16000000UL

#include <util/delay.h>
#include <math.h>
#include <stdio.h>
//FFT
#define FSAMPLE 2000UL
#define TS (1000000/FSAMPLE)-22

#define N 256UL// CANTIDAD DE MUESTRAS
#define N_out (N/2)+1

float REX[N];// PARTE REAL
float IMX[N];//  PARTE IMAGINARIA
float Mag[N];// MAGNITUDES

int I, IP, JM1, K, L, LE, LE2, NM1, ND2, M, J;
float TR, TI, UR, UI, SR, SI;

void ADC_init();
unsigned int ADC_read(unsigned char adc_input);
void capture_signal(unsigned char canal);
void ventana_hammin(); 
int max_indx();
void calc_FFT();
void prom_FFT();

//band
#define PRESCALER		64UL
#define F_MIN_HZ_VEL	127UL
#define F_MAX_HZ_VEL    1273UL
#define F_MIN_HZ_SND	700UL
#define F_MAX_HZ_SND	900UL

/* OCR1A para frecuencia objetivo con N=64 */
#define OCR_FROM_FREQ(f) ((uint16_t)((F_CPU / (2 * PRESCALER * (f))) - 1))

uint16_t mapping(int v_act);
void timer1_ctc_init(uint16_t f_target);
void timer1_set_freq(uint16_t f_target);
void working(void);

//UART
#define BAUD 57600
#define MYUBRR F_CPU/16/BAUD-1

void uart_init(unsigned int ubrr);
void uart_transmit(unsigned char data);
void uart_print(const char *str);
void envia_info();



#endif /* SETUP_H_ */