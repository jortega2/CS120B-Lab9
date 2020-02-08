/*	Author: jorte057
 *  Partner(s) Name: Duke Pham dpham073@ucr.edu
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//0.954 hz is lowest frequency possible with this function
//based on settings in PWM_on()
//Passing in 0 as the frequency will stop the speaker from generating sound

double mute = 0;
unsigned char i;
const double array[8] = { 261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

enum states {init, off, wait1, on, wait2, inc, incWait,  dec, decWait } state;

void tick() {
	switch(state) {
		case init:
			state = off;
			break;
		case off:
			if ((~PINA & 0x07) == 0x01){
				state = wait1;
			} else {
				state = off;
			}
			break;
		case on:
			if ( (~PINA & 0x07)  == 0x01){
				state = wait2;
			} else if ((~PINA & 0x07) == 0x02){
				/*if (i < 7){{
					i++;
				}*/
				state = inc;
			} else if ((~PINA & 0x07) == 0x04){
				/*if (i > 0){
					i--;
				}*/
				state = dec;
			} else {
				state = on;
			}
			break;
		case wait1:
			if ((~PINA & 0x07) == 0x01){
				state = wait1;
			} else {
				state = on;
			}
			break;
		case wait2:
			if ((~PINA & 0x07) == 0x01){
				state = wait2;
			}else {
				state = off;
			}
			break;
		case inc:
			state = incWait;
			break;
		case incWait:
			if ((~PINA & 0x07) == 0x02){
				state = incWait;
			} else {
				state = on;
			}
			break;
		case dec:
			state = decWait;
			break;
		case decWait:
			if ((~PINA & 0x07) == 0x04) {
				state = decWait;
			} else {
				state = on;
			}
			break;
	}
	switch (state) {
		case init: 
			break;
		case off:
			set_PWM(mute);
			break;
		case on:
			set_PWM(array[i]);
			break;
		case inc:
			if (i <7){
				i = i+ 1;
			}
			break;
		case incWait:
			break;
		case wait1:
			break;
		case wait2:
			break;
		case dec:
			if (i > 0) {
				i = i - 1;
			}
			break;
		case decWait:
			break;
		default:
			break;
	}
}

void set_PWM(double frequency) {
	static double current_frequency; //Keeps track of the currently set frequency
	//will only update the registers when the frequenc changes, otherwse allows
	//music to play uninterrupted
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08;}//stops timer/counter
		else {TCCR3B |= 0x03;}//resumes/continues timer/counter
	
		//prevents OCR3A from overflowing, using prescaler 64
		//0.954 is smallet frequency that will not result in overflow
		if (frequency < 0.954) {OCR3A = 0xFFFF;}
		
		//prevents OCRR3A from underflowing, using prescaler 64
		//31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) {OCR3A = 0x0000;}
		
		//set OCR3A based on desired frequency
		else {OCR3A = (short)(8000000 / (128 * frequency)) -1;}
		
		TCNT3 = 0;
		current_frequency = frequency;
	}
}
	
void PWM_on() {
	TCCR3A = (1 << COM3A0);
		//COM3A0: Toggle PB3 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
		//WGM32: When counter (TCNT3) matches OCR3A, reset counter
		//CS31 & CS30: Set a prescaler of 64
		set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0x40;
	DDRA = 0x00; PORTA = 0xFF;	
	PWM_on();
	i = 4;
    /* Insert your solution below */
    while (1) {
	tick();
    }
    return 1;
}
