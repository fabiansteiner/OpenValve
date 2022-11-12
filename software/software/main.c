/*
 * software.c
 *
 * Created: 15.10.2022 16:24:59
 * Author : Gus
 */ 


#include "common.h"


int main(void)
{
	
	

	PORTB_DIRSET = (1<<3); 
	PORTA_DIRSET = (1<<7); 
	PORTA_DIRSET = (1<<6); 
	uart_init(9600);
	
    while (1) 
    {
		
		/*
		
		_delay_ms(1000);
		PORTB_OUTSET = (1<<3); // set HIGH;
		PORTA_OUTSET = (1<<7); // set HIGH;
		PORTA_OUTSET = (1<<6); // set HIGH;
		_delay_ms(1000);
		PORTB_OUTCLR = (1<<3); // set HIGH;
		PORTA_OUTCLR = (1<<7);
		PORTA_OUTCLR = (1<<6);
		uart_sendstring("Hello World!\r\n");
		*/
		
		if(PORTB_IN & (1<<0)){
			PORTB_OUTSET = (1<<3); // set HIGH;
		}else{
			PORTB_OUTCLR = (1<<3);
		}
		
    }
}

