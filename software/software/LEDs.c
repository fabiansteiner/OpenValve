#define F_CPU 3333333

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdbool.h>


#include "LEDs.h"
#include "UserInterface.h"

volatile UIstate currentState;

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

volatile uint8_t thresholdLevel;





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
	if (currentState == TRANSITION){
		if (animationCounter < 4){
			PORTB.OUTTGL = (1<<PIN_GREENLED);
			animationCounter++;
		}else{
			//Turn off counter
			TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_OVF_bm;
			changeUIState(0);
		}
		
	}else if(currentState == SHOWBATTERY){
		cycleBatteryLevelAnimation();
	}else if(currentState == CHANGETHRESHOLD){
		if (animationCounter < thresholdLevel*2){
			PORTB.OUTTGL = (1<<PIN_GREENLED);
			PORTA.OUTTGL = (1<<PIN_REDLED);
			animationCounter++;
		}else{
			animationCounter++;
			if(animationCounter > thresholdLevel*2+8){
				animationCounter = 0;
			}
		}
	}else if (currentState == CHANGEINTERVAL){
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

	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;			//Single mode
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;	//Normal operation, no waveform or other extras
	TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTEI_bm);		//Count steps, not events
}

//Blink green led two times and then switch to passed state
void animateTransition(){

	currentState = TRANSITION;

	//Set overflow interval to 250ms
	resetTimerSettings();
	TCA0.SINGLE.PER = 1000;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;
}

void animateBatteryLevel(uint16_t ADCMeasurement){
	currentState = SHOWBATTERY;
	batteryLevel = ADCMeasurement;

	//Set overflow interval to 50khz
	resetTimerSettings();
	TCA0.SINGLE.PER = 1000;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
}

void animateSelectThreshold(){
	currentState = SELECTTHRESHOLD;

	//TODO: Later maybe Pulsate Brown
	resetTimerSettings();
	PORTB.OUTSET = (1<<PIN_GREENLED);
	PORTA.OUTSET = (1<<PIN_REDLED);
}

void animateSelectInterval(){
	currentState = SELECTINTERVAL;

	//Later maybe Pulsate RED
	resetTimerSettings();
	PORTA.OUTSET = (1<<PIN_REDLED);
}

void animateChangeSoilThreshold(uint16_t currentThresholdLevel){
	//Blink as many times as currentThresholdLevel, then stop for a second
	currentState = CHANGETHRESHOLD;
	thresholdLevel = currentThresholdLevel;

	resetTimerSettings();
	TCA0.SINGLE.PER = 2700;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;
}

void animateChangeInterval(uint16_t currentIntervalLevel){
	//Blink led red, the higher the interval level the faster it blinks
	currentState = CHANGEINTERVAL;

	resetTimerSettings();
	switch(currentIntervalLevel){
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
}