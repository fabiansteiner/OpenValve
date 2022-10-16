/*
 * software.c
 *
 * Created: 15.10.2022 16:24:59
 * Author : Gus
 */ 


#include "common.h"


int main(void)
{

	PORTA_DIRSET = (1<<7); //PA7 is LED
	uart_init(9600);
	
    while (1) 
    {
		
		_delay_ms(1000);
		PORTA_OUTSET = (1<<7); // set HIGH;
		_delay_ms(1000);
		PORTA_OUTCLR = (1<<7); // set HIGH;
		uart_sendstring("Hello World!\r\n");
		
    }
}

