/*
 * inputController.h
 *
 * Created: 02.11.2022 16:18:04
 *  Author: SteinerF
 *
 *	This file included the functions necessary to interface with the user. (Magnetic Switch, LEDs)
 */ 


#ifndef INPUTCONTROLLER_H_
#define INPUTCONTROLLER_H_

#define PIN_MAGNETSWITCH 0		//PORTB
#define MAINLOOP_DELAY 10


typedef enum {
	SELECTTHRESHOLD = 1, 
	SELECTINTERVAL= 2,
	CHANGETHRESHOLD = 3, 
	CHANGEINTERVAL = 4, 
	SHOWBATTERY = 5, 
	TRANSITION = 6,
	SHOWNOTHING = 7}  UIstate;
	
typedef enum {
	SEC4 = 5,
	SEC16 = 4,
	MIN1 = 3,
	MIN5 = 2,
	MIN60 = 1}  wakeUpInterval;
	
typedef struct{
	uint16_t tresholdOpen;
	uint16_t thresholdClose;
} thresholds;
	
void initUI();

void changeUIState(uint8_t longpress);

void senseMagneticSwitch();

void countUITimeOut();

thresholds getCurrentThresholds();

uint8_t getCurrentInterval();

UIstate getUIState();




#endif /* INPUTCONTROLLER_H_ */