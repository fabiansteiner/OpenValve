/*
 * software.c
 *
 * Created: 15.10.2022 16:24:59
 * Author : Gus
 */ 


#include "common.h"
#include "ADC.h"
#include <avr/interrupt.h>


int main(void)
{
	
	sei();

	PORTB_DIRSET = (1<<3); 
	PORTA_DIRSET = (1<<7); 
	uart_init(9600);
	initADC();
	initializeValve();
	
	
    while (1) 
    {
		/*
		//Switching motor state with the magnet
		if((PORTB_IN & (1<<0))==0){
			 _delay_ms(5);
			 while((PORTB_IN & (1<<0))==0);
			 changeMotorState();
		}
		*/
		
		if((PORTA_IN & (1<<PIN_MOTORSTOP))==0){
			PORTB_OUTSET = (1<<3);
		}else{
			PORTB_OUTCLR = (1<<3);
		}
		
		if(ADC_0_readSoilMoisture() >= 512){
			PORTA_OUTSET = (1<<7);
			if(getValveState() == OPEN || getValveState()== UNDEFINED){
				closeValve();
			}
		}else{
			PORTA_OUTCLR = (1<<7);
			if(getValveState() == CLOSED){
				openValve();
			}
		}
		
		_delay_ms(10);
		
    }
}

