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
	SHOWNOTHING = 6}  UIstate;
	
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

typedef enum {SHORT = 0, LONG = 1, NONE = 2} pressType;

typedef enum {
NO_CHANGE, 
FROM_SHOWNOTHING_TO_SHOWBATTERY,
FROM_SHOWBATTERY_TO_SELECTTHRESHOLD,
FROM_SELECTTHRESHOLD_TO_SELECTINTERVAL,
FROM_SELECTTHRESHOLD_TO_CHANGETHRESHOLD,
FROM_SELECTINTERVAL_TO_SELECTTHRESHOLD,
FROM_SELECTINTERVAL_TO_CHANGEINTERVAL,
THRESHOLD_CHANGED,
INTERVAL_CHANGED,
UI_OFF,
UI_OFF_WITHOUT_CONFIRMING
} state_change;


extern UIstate uiState;
extern UIstate previousState;
extern uint8_t soilLevel;
extern uint8_t interval;
	
void initUI();

state_change changeUIState(pressType button_press);

pressType senseMagneticSwitch();

state_change countUITimeOut();

thresholds getCurrentThresholds();

uint8_t getCurrentInterval();

UIstate getUIState();




#endif /* INPUTCONTROLLER_H_ */