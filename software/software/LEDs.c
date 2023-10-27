#define F_CPU 3333333

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdbool.h>


#include "LEDs.h"
#include "UserInterface.h"
#include "valve.h"
#include "ADC.h"


void (*func_ptr)(void);

volatile currentLEDAnimation ongoingAnimation = NO_ANIMATION;
volatile uint16_t animationCounter = 0;

#define ONESTEP 66
#define HALFSTEP 33
#define MAX_LEVEL 48*ONESTEP
#define MIN_LEVEL 2*ONESTEP

volatile uint16_t batteryLevel;
volatile uint8_t cycle = 0;
volatile uint8_t cycleCounter = 0;
volatile uint16_t greenBrightness = 132;
volatile uint8_t msCounter = 0;
volatile uint8_t countingUp = 1;


void showBlueLED();
void animateTransition();	//Blink green led two times
void animateBatteryLevel();		//From red to green to red to battery state
void animateSelectThreshold();	//Glow Orange
void animateSelectInterval();	//Glow Red
void animateChangeSoilThreshold();	//Blink as many times as currentThresholdLevel, then stop for a second
void animateChangeInterval();			//Blink led red, the higher the interval level the faster it blinks
void stopLEDs();


void cycleBatteryLevelAnimation(){
	
	//if + else = 1ms
	if (cycle == 0){
		PORTB.OUTSET = (1<<PIN_GREENLED);
		PORTA.OUTCLR = (1<<PIN_REDLED);
		TCA0.SINGLE.PERBUF = 3300-greenBrightness;
		cycle = 1;
	}else{
		PORTB.OUTCLR = (1<<PIN_GREENLED);
		PORTA.OUTSET = (1<<PIN_REDLED);
		TCA0.SINGLE.PERBUF = greenBrightness;
		cycle = 0;
	}
	
	cycleCounter++;
	//1ms passed
	if (cycleCounter >= 2) {
		cycleCounter = 0;
		
		if (countingUp != 3)
		{
			msCounter++;
			if(msCounter >= 20){
				msCounter = 0;
				
				if (countingUp==1){
						if(greenBrightness >= ONESTEP*10 || greenBrightness <= ONESTEP*40){
							greenBrightness = greenBrightness + HALFSTEP;
						}else{
							greenBrightness = greenBrightness + ONESTEP;
						}
						
						if(greenBrightness >= MAX_LEVEL) countingUp = 0;
					}else if (countingUp == 0){
						if(greenBrightness >= ONESTEP*10 || greenBrightness <= ONESTEP*40){
							greenBrightness = greenBrightness - HALFSTEP;
							}else{
							greenBrightness = greenBrightness - ONESTEP;
						}

						if(greenBrightness <= MIN_LEVEL) countingUp = 2;
					}else if (countingUp == 2){
						if (greenBrightness <= batteryLevel*66) {
							if(greenBrightness >= ONESTEP*10 || greenBrightness <= ONESTEP*40){
								greenBrightness = greenBrightness + HALFSTEP;
								}else{
								greenBrightness = greenBrightness + ONESTEP;
							}
						}
						else {countingUp = 3;}
					}
			}
		}
		
		
	}
	
}

ISR(TCA0_OVF_vect)
{
	if (ongoingAnimation == A_TRANSITIONING){
		if (animationCounter < 4){
			PORTB.OUTTGL = (1<<PIN_GREENLED);
			animationCounter++;
		}else{
			//Turn off counter
			TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_OVF_bm;
			(*func_ptr)(); //Call appropriate function that was assigned when changing the state
		}
		
	}else if(ongoingAnimation == A_BATTERY){
		cycleBatteryLevelAnimation();
	}else if(ongoingAnimation == A_CHANGETHRESHOLD){
		if (animationCounter < soilLevel*2){
			PORTB.OUTTGL = (1<<PIN_GREENLED);
			PORTA.OUTTGL = (1<<PIN_REDLED);
			animationCounter++;
		}else{
			animationCounter++;
			if(animationCounter > soilLevel*2+8){
				animationCounter = 0;
			}
		}
	}else if (ongoingAnimation == A_CHANGEINTERVAL){
		PORTA.OUTTGL = (1<<PIN_REDLED);
	}


	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

void resetTimerSettings(){
	// Hard reset timer
	TCA0.SINGLE.CTRLA = 0;
	TCA0.SINGLE.CNT = 0;
	TCA0.SINGLE.CTRLESET = TCA_SINGLE_CMD_RESET_gc;
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;

	//Reset LEDs
	PORTA.OUTCLR = (1<<PIN_REDLED);
	PORTB.OUTCLR = (1<<PIN_GREENLED);
	PORTB.OUTCLR = (1<<BLUE_LED);

	//Reset counters used for LED animation
	animationCounter = 0;

	cycle = 0;
	cycleCounter = 0;
	greenBrightness = 132;
	msCounter = 0;
	countingUp = 1;

	//Reinitialize Timer
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;			//Single mode
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;	//Normal operation, no waveform or other extras
	TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTEI_bm);		//Count steps, not events

	
}

void initLEDs(){
	PORTB_DIRSET = (1<<PIN_GREENLED);
	PORTA_DIRSET = (1<<PIN_REDLED);
	PORTB_DIRSET = (1<<BLUE_LED);

	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;			//Single mode
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;	//Normal operation, no waveform or other extras
	TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTEI_bm);		//Count steps, not events
}

void showBlueLED(){
	if(getValveState() == OPEN){
		PORTB_OUTSET = (1<<BLUE_LED);
		}else if (getValveState() == CLOSED){
		PORTB_OUTCLR = (1<<BLUE_LED);
	}
}

void cycleLEDAnimation(state_change change){
	
	showBlueLED();
	
	switch(change){
		case FROM_SHOWNOTHING_TO_SHOWBATTERY: animateBatteryLevel();
			break;
		case FROM_SHOWBATTERY_TO_SELECTTHRESHOLD: func_ptr = &animateSelectThreshold;				animateTransition();	//Transition
			break;
		case FROM_SELECTTHRESHOLD_TO_SELECTINTERVAL: animateSelectInterval();
			break;
		case FROM_SELECTTHRESHOLD_TO_CHANGETHRESHOLD: func_ptr = &animateChangeSoilThreshold;		animateTransition(); //Transition
			break;
		case FROM_SELECTINTERVAL_TO_SELECTTHRESHOLD: animateSelectThreshold();
			break;
		case FROM_SELECTINTERVAL_TO_CHANGEINTERVAL: func_ptr = &animateChangeInterval;				animateTransition(); //Transition
			break;
		case THRESHOLD_CHANGED: animateChangeSoilThreshold();
			break;
		case INTERVAL_CHANGED: animateChangeInterval();
			break;
		case UI_OFF:  func_ptr = &stopLEDs; animateTransition();	//Transition
			break;
		case UI_OFF_WITHOUT_CONFIRMING: stopLEDs();	//Transition
			break;
		default:
			break;
		
	}

	

	

}


//Blink green led two times and then switch to passed state
void animateTransition(){

	ongoingAnimation = A_TRANSITIONING;

	//Set overflow interval to 250ms
	resetTimerSettings();
	TCA0.SINGLE.PER = 1000;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;

	
	
}

void animateBatteryLevel(){

	
	batteryLevel = getBatteryLevel();

	ongoingAnimation = A_BATTERY;

	//Set overflow interval to 50khz
	resetTimerSettings();
	TCA0.SINGLE.PER = 1000;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
}

void animateSelectThreshold(){

	
	ongoingAnimation = A_SELECTTHRESHOLD;

	resetTimerSettings();
	PORTB.OUTSET = (1<<PIN_GREENLED);
	PORTA.OUTSET = (1<<PIN_REDLED);
}

void animateSelectInterval(){


	ongoingAnimation = A_SELECTINTERVAL;

	resetTimerSettings();
	PORTA.OUTSET = (1<<PIN_REDLED);
}

void animateChangeSoilThreshold(){
	ongoingAnimation = A_CHANGETHRESHOLD;
	
	//Blink as many times as currentThresholdLevel, then stop for a second
	resetTimerSettings();
	TCA0.SINGLE.PER = 2700;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;
}

void animateChangeInterval(){
	ongoingAnimation = A_CHANGEINTERVAL;
	
	//Blink led red, the higher the interval level the faster it blinks%
	resetTimerSettings();
	switch(interval){
		case 1: TCA0.SINGLE.PER = 24000;
			break;
		case 2: TCA0.SINGLE.PER = 12000;
			break;
		case 3: TCA0.SINGLE.PER = 6000;
			break;
		case 4: TCA0.SINGLE.PER = 3000;
			break;
		case 5: TCA0.SINGLE.PER = 1500;
			break;
	}

	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;
	
}

void stopLEDs(){
	resetTimerSettings();
	ongoingAnimation = NO_ANIMATION;
}

currentLEDAnimation getLEDAnimation(){
	return ongoingAnimation;
}