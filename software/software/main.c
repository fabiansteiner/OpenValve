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
		
		if((PORTB_IN & (1<<0))==0){
			 _delay_ms(5);
			 while((PORTB_IN & (1<<0))==0);
			 changeMotorState();
		}
		
		if(getValveState() == OPEN){
			//PORTB_OUTSET = (1<<3);
		}else if (getValveState() == CLOSED){
			//PORTB_OUTCLR = (1<<3);
		}
		
		if(ADC_0_readSoilMoisture() >= 512){
			PORTA_OUTSET = (1<<7);
		}else{
			PORTA_OUTCLR = (1<<7);
		}
		
		_delay_ms(5);
		
    }
}

