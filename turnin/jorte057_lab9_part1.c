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

double off = 0;
double c4 = 261.63;
double d4 = 293.66;
double e4 = 329.23;

enum states {init, interphase, C4, D4, E4 } state;

void tick() {
	switch(state) {
		case init:
			state = interphase;
			break;
		case interphase:
			if ((~PINA & 0x07) == 0x01){
				state = C4;
			} else if ( (~PINA & 0x07)  == 0x02) {
				state = D4;
			} else if ( (~PINA & 0x07)  == 0x04) {
				state = E4;
			} else {
				state = interphase;
			}
			break;
		case C4:
			if ( (~PINA & 0x07)  == 0x01){
				state = C4;
			} else {
				state = interphase;
			}
			break;
		case D4:
			if ((~PINA & 0x07)  == 0x02){
				state = D4;
			} else {
				state = interphase;
			}
			break;
		case E4:
			if ((~PINA & 0x07)  == 0x04){
				state = E4;
			}else{
				state = interphase;
			}
			break;
	}
	switch (state) {
		case init: 
			break;
		case interphase:
			set_PWM(off);
			break;
		case C4:
			set_PWM(c4);
			break;
		case D4:
			set_PWM(d4);
			break;
		case E4:
			set_PWM(e4);
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
    /* Insert your solution below */
    while (1) {
	tick();
    }
    return 1;
}
