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

volatile valveState motState = OPEN;

void initializeValve(){
	
	PORTB_DIRSET = (1<<PIN_MOTORPLUS);
	PORTA_DIRSET = (1<<PIN_MOTORMINUS);
	
	PORTA.DIR &= ~ PIN5_bm;												//Set as input
	PORTA.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;			//Enable Pull-UP & enable interrupt on falling edge
	
	_delay_ms(100);														//Let the pull-up take effect


	if((PORTA_IN & (1<<PIN_MOTORSTOP))==0){
		motState = CLOSED;
	}

	
}

void openValve(){
	if(motState == CLOSED){
		//drive motor for 50ms
		motState = OPENING;
		PORTA_OUTSET = (1<<PIN_MOTORMINUS); // set HIGH;

		_delay_ms(50);
		ADC_0_startMotorCurrentCheck();
		_delay_ms(500);

		PORTA_OUTCLR = (1<<PIN_MOTORMINUS);
		motState = OPEN;
		_delay_ms(100);			//Let the motor calm down before driving it in the other direction
	}
}


void closeValve(){

	if(motState == OPEN || motState == UNDEFINED){
		motState = CLOSING;
		PORTB_OUTSET = (1<<PIN_MOTORPLUS); // set HIGH;
		
		_delay_ms(100);

		ADC_0_startMotorCurrentCheck();
		
		
		//Wait until actually closed
		//_delay_ms(1500);	//Workaround because checking if motState is closed does not work for whatever reason
		while(motState == CLOSING);
		//_delay_ms(20);


		//TODO: Add Timeout here
	}
	
}

void changeMotormotState(){
	if(motState == OPEN || motState == UNDEFINED){
		closeValve();
	}else if(motState == CLOSED){
		openValve();
	}
}


valveState getValveState(){
	return motState;
}

void setValveState(valveState vState){
	motState = vState;
}

ISR(PORTA_PORT_vect)
{
	if(PA5_INTERRUPT && motState == CLOSING)
	{
		PORTB_OUTCLR = (1<<PIN_MOTORPLUS);
		motState = CLOSED;
		
	}
	PA5_CLEAR_INTERRUPT_FLAG;
}

