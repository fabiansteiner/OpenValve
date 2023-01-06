/*
 * valve.c
 *
 * Created: 30.11.2022 13:51:29
 *  Author: SteinerF
 */ 

#define F_CPU 3333333
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "valve.h"
#include "ADC.h"

#define PA5_INTERRUPT PORTA.INTFLAGS & PIN5_bm
#define PA5_CLEAR_INTERRUPT_FLAG PORTA.INTFLAGS &= PIN5_bm

uint8_t state = UNDEFINED;

void initializeValve(){
	
	PORTB_DIRSET = (1<<PIN_MOTORPLUS);
	PORTA_DIRSET = (1<<PIN_MOTORMINUS);
	
	PORTA.DIR &= ~ PIN5_bm;												//Set as input
	PORTA.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;			//Enable Pull-UP & enable interrupt on falling edge

	if((PORTA_IN & (1<<PIN_MOTORSTOP))==0){
		state = CLOSED;
	}
	
}

void openValve(){
	if(state == CLOSED){
		//drive motor for 50ms
		state = OPENING;
		PORTA_OUTSET = (1<<PIN_MOTORMINUS); // set HIGH;
		_delay_ms(50);
		ADC_0_startMotorCurrentCheck();
		_delay_ms(300);
		PORTA_OUTCLR = (1<<PIN_MOTORMINUS);
		state = OPEN;
		_delay_ms(500);
	}
}


void closeValve(){
	//drive motor as long as button is pressed (timeout 2s)
	if(state == OPEN || state == UNDEFINED){
		state = CLOSING;
		PORTB_OUTSET = (1<<PIN_MOTORPLUS); // set HIGH;
		
		//When Closing make 50ms delay until current sensing goes (because of current spike when turning on motor) 
		//but still check if button is pressed if for whatever reason the valve stands closely before the button
		uint16_t i = 0;
		for (i=0; i<500; i++)
		{
			if((PORTA_IN & (1<<PIN_MOTORSTOP))==0){
				PORTB_OUTCLR = (1<<PIN_MOTORPLUS);
				state = CLOSED;
				break;
			}
			_delay_us(100);
		}

		//After 50ms turn on current sensing
		ADC_0_startMotorCurrentCheck();

		//Add Timeout here
	}
	
}

void changeMotorState(){
	if(state == OPEN || state == UNDEFINED){
		closeValve();
	}else if(state == CLOSED){
		openValve();
	}
}


valveState getValveState(){
	return state;
}

void setValveState(valveState vState){
	state = vState;
}

ISR(PORTA_PORT_vect)
{
	if(PA5_INTERRUPT && state == CLOSING)
	{
		PORTB_OUTCLR = (1<<PIN_MOTORPLUS);
		state = CLOSED;
		
	}
	PA5_CLEAR_INTERRUPT_FLAG;
}

