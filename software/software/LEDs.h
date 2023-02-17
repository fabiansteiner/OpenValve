/*
 * LEDs.h
 *
 * Created: 02.11.2022 16:17:20
 *  Author: SteinerF
 */ 


#ifndef LEDS_H_
#define LEDS_H_

void initLEDs();

void animateTransition();	//Blink green led two times

void animateSelectThreshold();	//Pulsate Brown

void animateSelectInterval();	//Pulsate RED

void animateChangeSoilThreshold(uint16_t currentThresholdLevel);	//Blink as many times as currentThresholdLevel, then stop for a second

void animateChangeInterval(uint16_t currentIntervalLevel);			//Blink led red, the higher the interval level the faster it blinks

void stopLEDs();



#endif /* LEDS_H_ */