/*
 * setup.h
 *
 * Created: 17/02/2026 11:31:27 a. m.
 *  Author: Institucional
 */ 


#ifndef SETUP_H_
#define SETUP_H_

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>

#define FSAMPLE 4000
#define TS (1000000/FSAMPLE)-22
#define BAUD 57600
#define MYUBRR F_CPU/16/BAUD-1
#define N 64// CANTIDAD DE MUESTRAS
#define N_out (N/2)+1
float REX[N];// PARTE REAL
float IMX[N];//  PARTE IMAGINARIA
float Mag[N];// MAGNITUDES
int I, IP, JM1, K, L, LE, LE2, NM1, ND2, M, J;
float TR, TI, UR, UI, SR, SI;

void uart_init(unsigned int ubrr);
void ADC_init();
unsigned int ADC_read(unsigned char adc_input);
void uart_transmit(unsigned char data);
void uart_print(const char *str);
void capture_signal(unsigned char canal);
void envia_info();
void calc_FFT();



#endif /* SETUP_H_ */