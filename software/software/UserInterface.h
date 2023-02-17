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

typedef enum {
	SELECTTHRESHOLD = 1, 
	SELECTINTERVAL= 2,
	CHANGETHRESHOLD = 3, 
	CHANGEINTERVAL = 4, 
	SHOWBATTERY = 5, 
	CHANGESTATE = 6}  UIstate;
	
typedef struct{
	uint16_t tresholdLow;
	uint16_t thresholdHigh;
} thresholds;
	
void initUI();


thresholds getCurrentThresholds();

uint32_t getIntervalMS();





#endif /* INPUTCONTROLLER_H_ */