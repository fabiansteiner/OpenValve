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

	initLEDs();
	uart_init(9600);
	//initADC();
	//initializeValve();
	
	uint8_t level = 1;
	animateChangeSoilThreshold(8);
    while (1) 
    {
		
		/*
		animateBatteryLevel(level*8);
		_delay_ms(10000);
		animateTransition(SHOWBATTERY);
		_delay_ms(500);
		animateSelectThreshold();
		_delay_ms(2000);
		animateTransition(SHOWBATTERY);
		_delay_ms(500);
		animateChangeSoilThreshold(level);
		_delay_ms(10000);
		animateTransition(SHOWBATTERY);
		_delay_ms(500);
		stopLEDs();
		_delay_ms(10000);
		*/

		level++;
		if (level > 5) level=1;


		/*
		if(ADC_0_readSoilMoisture() >= 512){
			//PORTA_OUTSET = (1<<7);
			if(getValveState() == OPEN || getValveState()== UNDEFINED){
				closeValve();
			}
		}else{
			//PORTA_OUTCLR = (1<<7);
			if(getValveState() == CLOSED){
				openValve();
			}
		}
		
		
		_delay_ms(10);
		*/
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