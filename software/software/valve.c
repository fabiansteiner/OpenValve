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
#include "LEDs.h"

#define PA5_INTERRUPT PORTA.INTFLAGS & PIN5_bm
#define PA5_CLEAR_INTERRUPT_FLAG PORTA.INTFLAGS &= PIN5_bm



volatile valveState motState = OPEN;
volatile valveError error = NO_ERROR;

void initializeValve(){
	
	PORTB_DIRSET = (1<<PIN_MOTORPLUS);
	PORTA_DIRSET = (1<<PIN_MOTORMINUS);
	
	PORTA.DIR &= ~ PIN5_bm;												//Set as input
	PORTA.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;			//Enable Pull-UP & enable interrupt on falling edge
	
	_delay_ms(100);														//Let the pull-up take effect


	if((PORTA_IN & (1<<PIN_MOTORSTOP))==0){
		motState = CLOSED;
	}else{
		PORTB_OUTSET = (1<<BLUE_LED);
	}

	
}


void openValve(){
	if(motState == CLOSED && error == NO_ERROR){

		uint16_t voltageADC;
		uint16_t currentADC;
		float calc_volt = 5.0;
		float calc_curr;
		float calc_watt;
		uint16_t timeCounter = 0;

		motState = OPENING;
		PORTA_OUTSET = (1<<PIN_MOTORMINUS); // set HIGH;

		_delay_ms(200);
		while(motState == OPENING && timeCounter <= 1200){

			currentADC = ADC_0_readCurrent();
			calc_curr  = (currentADC * MAX_CUR) / RES_10BIT;
			voltageADC = ADC_0_readBatteryVoltage();
			calc_volt  = (voltageADC * MAX_VOL) / RES_10BIT;
			
			calc_watt = calc_volt * calc_curr;

			if(calc_watt >= 3.8 ){	//(calc_volt >= 6.5 && calc_watt >= 2.0) || (calc_volt < 6.5 && calc_watt >= 2.5)
				PORTA_OUTCLR = (1<<PIN_MOTORMINUS);
				motState = OPEN;
			}

			timeCounter++;

		}
		PORTA_OUTCLR = (1<<PIN_MOTORMINUS);
		motState = OPEN;
		

		if(timeCounter>=1200){
			error = VALVE_TIMEOUT;
			//PORTA_OUTSET = (1<<PIN_REDLED);
		}
		
		PORTB_OUTSET = (1<<BLUE_LED);
		//IGNORE ERROR STATES FOR DEV
		error = NO_ERROR;
		_delay_ms(100);			//Let the motor calm down before driving it in the other direction
	}
}


void closeValve(){
	
	if(motState == OPEN && error == NO_ERROR){
		
		uint16_t voltageADC;
		uint16_t currentADC;
		float calc_volt = 5.0;
		float calc_curr;
		float calc_watt;
		uint16_t timeCounter = 0;

		
		
		motState = CLOSING;
		PORTB_OUTSET = (1<<PIN_MOTORPLUS); // set HIGH;
		_delay_ms(150);

		while(motState == CLOSING && timeCounter <= 1400){

			currentADC = ADC_0_readCurrent();
			calc_curr  = (currentADC * MAX_CUR) / RES_10BIT;
			voltageADC = ADC_0_readBatteryVoltage();
			calc_volt  = (voltageADC * MAX_VOL) / RES_10BIT;
			
			calc_watt = calc_volt * calc_curr;

			if(calc_watt >= 5.5){
				PORTB_OUTCLR = (1<<PIN_MOTORPLUS);
				motState = CLOSED;
				error = HIGH_CURRENT;

				//PORTB_OUTSET = (1<<PIN_GREENLED);
			}

			timeCounter++;

		}
		
		if(timeCounter>=1400){
			PORTB_OUTCLR = (1<<PIN_MOTORPLUS);
			motState = CLOSED;
			error = VALVE_TIMEOUT;
			//PORTA_OUTSET = (1<<PIN_REDLED);
		}

		if(calc_volt <= 4.0){	//Triggers at 4.05
			//PORTB_OUTSET = (1<<BLUE_LED);
			motState = CLOSED;
			error = LOW_VOLTAGE;
		}
	
		PORTB_OUTCLR = (1<<BLUE_LED);
		//IGNORE ERROR STATES FOR DEV
		error = NO_ERROR;
		_delay_ms(100);			//Let the motor calm down before driving it in the other direction
		
	
	}
	
}

void changeMotorState(){
	if(error == NO_ERROR){
		if(motState == OPEN){
			closeValve();
		}else if(motState == CLOSED){
			openValve();
		}
	}
}


valveState getValveState(){
	return motState;
}

valveError getValveError(){
	return error;
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


