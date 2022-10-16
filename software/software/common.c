/*
 * common.c
 *
 * Created: 16.10.2022 11:28:57
 *  Author: Gus
 */ 


#include "common.h"


void uart_init(uint32_t baudrate)
{
	//UART TX (PB2) must be set as output
	PORTB_DIRSET = (1<<2);
	USART0_BAUD = (uint16_t)USART1_BAUD_RATE(baudrate);
	USART0.CTRLB |= USART_TXEN_bm;						//enable only transmitter
}


void uart_transmit(uint8_t c)
{
	while (!(USART0.STATUS & USART_DREIF_bm));
	USART0.TXDATAL = c;
}


void uart_sendstring(char * str)
{
	for(size_t i = 0; i < strlen(str); i++)
	{
		uart_transmit(str[i]);
	}
}
