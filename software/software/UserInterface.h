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

#define UI_TIMEOUT 30


typedef enum {
	SELECTTHRESHOLD = 1, 
	SELECTMULTIPLICATOR= 2,
	CHANGETHRESHOLD = 3, 
	CHANGEMULTIPLICATOR = 4, 
	SHOWBATTERY = 5, 
	SHOWSOILMOISTURE = 6,
	MANUALIRRIGATION = 7,
	SHOWNOTHING = 8,
	ERRORSTATE}  UIstate;
	
typedef enum {
	SEC4 = 5,
	SEC16 = 4,
	MIN1 = 3,
	MIN5 = 2,
	MIN60 = 1}  wakeUpMultiplicator;
	
typedef struct{
	uint16_t tresholdOpen;
	uint16_t thresholdClose;
} thresholds;

typedef enum {SHORT = 0, LONG = 1, NONE = 2, VERYLONG = 3, TRIPLESHORT = 4} pressType;

typedef enum {
NO_CHANGE, 
FROM_SHOWNOTHING_TO_SHOWBATTERY,
FROM_SHOWBATTERY_TO_SELECTTHRESHOLD,
FROM_SHOWBATTERY_TO_SHOWSOILMOISTURE,
FROM_SHOWSOILMOISTURE_TO_SHOWBATTERY,
FROM_SHOWSOILMOISTURE_TO_MANUALIRRIGATION,
FROM_MANUALIRRIGATION_TO_SHOWSOILMOISTURE,
FROM_SELECTTHRESHOLD_TO_SELECTMULTIPLICATOR,
FROM_SELECTTHRESHOLD_TO_CHANGETHRESHOLD,
FROM_SELECTMULTIPLICATOR_TO_SELECTTHRESHOLD,
FROM_SELECTMULTIPLICATOR_TO_CHANGEMULTIPLICATOR,
THRESHOLD_CHANGED,
MULTIPLICATOR_CHANGED,
UI_OFF,
UI_OFF_WITHOUT_CONFIRMING,
UI_SHUTDOWN,
UI_STARTUP,
SHOW_ERROR,
FROM_SHOW_ERROR_TO_SHOWBATTERY

} state_change;


extern UIstate uiState;
extern uint8_t soilLevel;
extern uint8_t multiplicator;
	
void initUI();

state_change changeUIState(pressType button_press);

pressType senseMagneticSwitch();

state_change countUITimeOut();

thresholds getCurrentThresholds();

uint8_t getCurrentMultiplicator();

UIstate getUIState();




#endif /* INPUTCONTROLLER_H_ */