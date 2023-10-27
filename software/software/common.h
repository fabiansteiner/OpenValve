/*
 * common.h
 *
 * Created: 16.10.2022 11:26:20
 *  Author: Gus
 */ 


#ifndef COMMON_H_
#define COMMON_H_

//include all modules
#define F_CPU 3333333

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "valve.h"

#define DEBUG_ON 1
#define DEBUG_OFF 2

#define DEBUG_MODE DEBUG_OFF

#define USART1_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

////////////////////////////////////////////////////////////////////////
/////////// UART functions 
////////////////////////////////////////////////////////////////////////


void uart_init(uint32_t baudrate);


void uart_transmit(uint8_t c);


void uart_sendstring(char * str );


#endif /* COMMON_H_ */