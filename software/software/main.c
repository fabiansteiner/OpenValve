/*
 * software.c
 *
 * Created: 15.10.2022 16:24:59
 * Author : Gus
 */ 

#define F_CPU 3333333

#include "common.h"
#include "ADC.h"
#include "LEDs.h"
#include <avr/interrupt.h>


int main(void)
{
	
	sei();
	
	PORTA_OUTSET = (1<<PIN_SOILSENSORON);	//Switch on soil mositure sensor
	_delay_ms(50);
	
	initLEDs();
	//uart_init(9600);
	initUI();
	initADC();
	initializeValve();
	
	uint16_t SM;
	
	
	
    while (1) 
    {
		
		senseMagneticSwitch();
		countUITimeOut();
		_delay_ms(MAINLOOP_DELAY);
		
		SM = ADC_0_readSoilMoisture();
		
	
		if(SM >= getCurrentThresholds().thresholdClose){
			if(getValveState() == OPEN || getValveState()== UNDEFINED){
				closeValve();
			}
		}else if (SM <= getCurrentThresholds().tresholdOpen){
			if(getValveState() == CLOSED){
				openValve();
			}
		}
		
		
		
		
		
	}
}


/*

		//Switch LED with the magnet
		if((PORTB_IN & (1<<0))==0){
			PORTA_OUTSET = (1<<7);
			}else{
			PORTA_OUTCLR = (1<<7);
		}

		//Switch LED when motor stop button is pressed
		if((PORTA_IN & (1<<PIN_MOTORSTOP))==0){
			PORTB_OUTSET = (1<<3);
			}else{
			PORTB_OUTCLR = (1<<3);
		}

		//Switching motor state with the magnet
		if((PORTB_IN & (1<<0))==0){
			 _delay_ms(5);
			 while((PORTB_IN & (1<<0))==0);
			 changeMotorState();
		}
		*/