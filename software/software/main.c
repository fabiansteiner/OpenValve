/*
 * software.c
 *
 * Created: 15.10.2022 16:24:59
 * Author : Gus
 */ 

#define F_CPU 3333333
#define MULTIPLICATOR_TIME_LIMIT 7200	//2 Stunden

#include "common.h"
#include "ADC.h"
#include "LEDs.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/atomic.h>

typedef enum {
	ACTIVE = 1,
	PERIODICWAKEUP = 2,
	SLEEP = 3,
	OFF = 4
}  mainStates;


volatile mainStates mState = SLEEP;
volatile uint8_t wakeUpCycles = 1;
volatile uint8_t sleepCounter = 0;
volatile uint8_t manualIrrigation = 0;
volatile uint8_t buttonSensingOn = 0;

volatile uint16_t irrigationTimeCounter = 0;
volatile uint8_t currentSleepTime = 4;
uint16_t miliSecCounter = 0;
uint8_t executeMultiplicator = 0;




#define PB0_HIGH (PORTB.IN & PIN0_bm)
#define PB0_INTERRUPT PORTB.INTFLAGS & PIN0_bm
#define PB0_CLEAR_INTERRUPT_FLAG PORTB.INTFLAGS &= PIN0_bm

void enablePITInterrupt(){
	RTC.PITINTCTRL |= (1<<0);
}
void disablePITInterrupt(){
	RTC.PITINTCTRL &= ~(1<<0);
}

void enablePORTBInterrupt(){
	PORTB.PIN0CTRL |=  PORT_ISC_BOTHEDGES_gc;
}

void disablePORTBInterrupt(){
	PORTB.PIN0CTRL &=  ~PORT_ISC_BOTHEDGES_gc;
}


ISR(RTC_PIT_vect)
{
	
	if(mState != ACTIVE){
		sleepCounter++;
		if(sleepCounter >= wakeUpCycles){
			disablePORTBInterrupt();
			mState = PERIODICWAKEUP;
			
		}
		
	}
	RTC.PITINTFLAGS = RTC_PI_bm;
	PB0_CLEAR_INTERRUPT_FLAG;
	
}


ISR(PORTB_PORT_vect)
{
	
	if(PB0_INTERRUPT)
	{
		if(PB0_HIGH){
			if(mState != ACTIVE && mState != PERIODICWAKEUP){
				buttonSensingOn = 0;
				disablePITInterrupt();
				
				if(mState == SLEEP){

					mState = ACTIVE;
					irrigationTimeCounter = 0;

					if(getValveError() == NO_ERROR){
						state_change changeOfState = changeUIState(SHORT);
						changeLEDAnimation(changeOfState);
					}else{
						//Direct access to UI state, because changeUIState only takes button presses an input and this input is not triggered from the user
						uiState = ERRORSTATE;
						changeLEDAnimation(SHOW_ERROR);
					}

					
				}
				
			}
		}else{
			buttonSensingOn = 1;
		}
		PB0_CLEAR_INTERRUPT_FLAG;
		RTC.PITINTFLAGS = RTC_PI_bm;
	}
	
}

void initSleep(void)
{
	SLPCTRL.CTRLA |= SLPCTRL_SMODE_PDOWN_gc;
	SLPCTRL.CTRLA |= SLPCTRL_SEN_bm;
	
	PORTB.DIR &= ~ PIN0_bm;												//Set as input
	PORTB.PIN0CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;			//Enable Pull-UP & enable interrupt on falling edge

	//configure RTC PIT
	RTC.CLKSEL |= RTC_CLKSEL_INT1K_gc;					//Use 1024Hz clock for RTC
	RTC.PITINTCTRL |= (1<<0);							//Enable Periodic Interrupt

	//while((RTC.PITSTATUS & (1<<0)) != 0);				//Wait until a possibly ongoing synchronization of RTC.PITCTRLA register is done.
	//RTC.PITCTRLA |= RTC_PRESCALER_DIV2048_gc | (1<<0);	//Set Periodic Interrupt Cycle to 1024, interrupt should happen now every second.


}


void changePIT(uint8_t prescaler, uint8_t cycles){
	while((RTC.PITSTATUS & (1<<0)) != 0);
	RTC.PITCTRLA = prescaler | (1<<0);
	while((RTC.PITSTATUS & (1<<0)) != 0);
	wakeUpCycles = cycles;
}

void changePITInterval(){
	if(getValveState() == OPEN){
		if(irrigationTimeCounter <= 180){ //180
			changePIT(RTC_PRESCALER_DIV2048_gc, 1); currentSleepTime = 4;	//4 seconds
		}else if (irrigationTimeCounter <= 592){
			changePIT(RTC_PRESCALER_DIV8192_gc, 1); currentSleepTime = 16;	//16 seconds
		}else{
			changePIT(RTC_PRESCALER_DIV16384_gc, 2); currentSleepTime = 60;	//1 minute
		}


	}else if(getValveState() == CLOSED){
		//changePIT(RTC_PRESCALER_DIV2048_gc, 1);	//for debugging 4 seconds
		changePIT(RTC_PRESCALER_DIV16384_gc, 60); //30min
	
	}
	
}

void switchOFF(){
	disablePITInterrupt();
	closeValve();
	PORTB_OUTCLR = (1<<BLUE_LED);
	manualIrrigation = 0;
	while(getLEDAnimation() != NO_ANIMATION);
	mState = OFF;
	sleep_mode();
}


int main(void)
{
	sei();
	
	
	initSleep();
	initLEDs();
	//uart_init(9600);
	initUI();
	initADC();
	initializeValve();
	changePITInterval();
	
	_delay_ms(100);			//Let setting settle in
	uint16_t SM;
	uint8_t motorStateChanged = 0;

	
	
	sleep_mode();
	
    while (1) 
    {
		if (mState == ACTIVE || mState == PERIODICWAKEUP){
			
			//When valve open, set blue led
			if(manualIrrigation == 0){
				if(getValveState() == OPEN) PORTB_OUTSET = (1<<BLUE_LED);
				else PORTB_OUTCLR = (1<<BLUE_LED);
			}
			
			
			if(getValveError()==NO_ERROR){

			
				if(mState == ACTIVE){
				
					pressType button_press = NONE;
				
					if(buttonSensingOn){
						button_press = senseMagneticSwitch();
					}
					
				
					state_change changeOfState = changeUIState(button_press);
				
					//Do not count timeout when in manual irrigation mode
					if(getUIState()!=MANUALIRRIGATION){
						state_change timeOutStateChange = countUITimeOut();
						if(timeOutStateChange == UI_OFF || timeOutStateChange == UI_OFF_WITHOUT_CONFIRMING){changeOfState = timeOutStateChange;}
					}
				
					if(changeOfState == FROM_SHOWBATTERY_TO_MANUALIRRIGATION){
						manualIrrigation = 1;
						openValve();
					}else if (changeOfState == FROM_MANUALIRRIGATION_TO_SHOWBATTERY){
						manualIrrigation = 0;
						closeValve();
						irrigationTimeCounter=0;
						miliSecCounter=0;
					}
				
					changeLEDAnimation(changeOfState);
					cycleLEDAnimation();
				
					if(changeOfState == UI_SHUTDOWN){switchOFF(); continue;}
				}

			
				//If manual irrigation off = normal operation
				if(manualIrrigation == 0 && executeMultiplicator == 0){
					_delay_ms(MAINLOOP_DELAY/2);
					PORTA_OUTSET = (1<<PIN_SOILSENSORON);	//Turn on soil mositure sensor, takes around 5ms to get stable measurement
					_delay_ms(MAINLOOP_DELAY/2);
					SM = ADC_0_readSoilMoisture();
					PORTA_OUTCLR = (1<<PIN_SOILSENSORON);	//Turn off soil mositure sensor
				
					if(SM >= getCurrentThresholds().thresholdClose){
						if(getValveState() == OPEN){
							if(mState == ACTIVE || getCurrentMultiplicator() == 1){
								closeValve();
								motorStateChanged = 1;
								irrigationTimeCounter = 0;
								executeMultiplicator = 0;
							}else{
								
								uint16_t newIrrigationTimeCounter = irrigationTimeCounter * (0.5f * (getCurrentMultiplicator()-1));
								
								//Limit total irrigation time to 2 hours
								if(irrigationTimeCounter > MULTIPLICATOR_TIME_LIMIT){
									closeValve();
									irrigationTimeCounter = 0;
								}else if ((irrigationTimeCounter + newIrrigationTimeCounter) > MULTIPLICATOR_TIME_LIMIT){
									irrigationTimeCounter = MULTIPLICATOR_TIME_LIMIT-irrigationTimeCounter;	
									executeMultiplicator = 1;
								}else{
									irrigationTimeCounter = newIrrigationTimeCounter;
									executeMultiplicator = 1;
								}
									
								
								
							}
							
						}
					}else if (SM <= getCurrentThresholds().tresholdOpen){
						if(getValveState() == CLOSED){
							openValve();
							motorStateChanged = 1;
						}
					}
				//If manual irrigation on = just wait, measuring is not necessary
				}else if(manualIrrigation == 1 && executeMultiplicator == 0){
					_delay_ms(MAINLOOP_DELAY);
					miliSecCounter += MAINLOOP_DELAY;
					if(miliSecCounter >=1000){
						irrigationTimeCounter++;
						miliSecCounter=0;
						
						if(irrigationTimeCounter >= 900){	//15min timout
							//Simulate button press
							changeUIState(SHORT);
							
							manualIrrigation = 0;
							closeValve();
							irrigationTimeCounter=0;
							miliSecCounter=0;
							
							changeLEDAnimation(FROM_MANUALIRRIGATION_TO_SHOWBATTERY);
							cycleLEDAnimation();
						}
					}
					
					
					
				}else if (executeMultiplicator == 1){
					// Close, when multiplicator executed or state changes to active
					if(irrigationTimeCounter < 5 || mState == ACTIVE){
						closeValve();
						executeMultiplicator = 0;
						irrigationTimeCounter = 0;
					}
					
				}

				//If error occured after driving the motor, change to error state and exit loop
				if(getValveError() != NO_ERROR){
					//PIT interval to 4 sec
					changePIT(RTC_PRESCALER_DIV2048_gc, 1);
					uiState = ERRORSTATE;
					if(mState == ACTIVE) changeLEDAnimation(SHOW_ERROR);
					continue;
				}

			
				if(mState == PERIODICWAKEUP){
					
					changePITInterval();
					if (getValveState() == OPEN && executeMultiplicator == 0) {
						if (irrigationTimeCounter < MULTIPLICATOR_TIME_LIMIT+10)
						irrigationTimeCounter += currentSleepTime;
					}else if (executeMultiplicator == 1 && irrigationTimeCounter >= currentSleepTime){
						irrigationTimeCounter -= currentSleepTime;
					}
					
					
						
					
					
					sleepCounter = 0;
					motorStateChanged = 0;
					mState = SLEEP;
					PORTB_OUTCLR = (1<<BLUE_LED);
					enablePORTBInterrupt();
					sleep_mode();
				
				}else if(mState == ACTIVE){
				
					if(getLEDAnimation() == NO_ANIMATION){
						changePITInterval();
						sleepCounter = 0;
						motorStateChanged = 0;
						mState = SLEEP;
						PORTB_OUTCLR = (1<<BLUE_LED);
						enablePITInterrupt();
						sleep_mode();
					}
				
				}
			}else{
			//valve error occured
				if (mState == ACTIVE){
					//Show the errors
					senseMagneticSwitch();

					
					state_change timeOutStateChange = countUITimeOut();
					

					_delay_ms(MAINLOOP_DELAY);

					changeLEDAnimation(timeOutStateChange);
					cycleLEDAnimation();

					if(getLEDAnimation() == NO_ANIMATION){
						sleepCounter = 0;
						mState = SLEEP;
						PORTB_OUTCLR = (1<<BLUE_LED);
						enablePITInterrupt();
						sleep_mode();
					}



				}else if(mState == PERIODICWAKEUP){
					PORTA.OUTSET = (1<<PIN_REDLED);
					_delay_ms(MAINLOOP_DELAY);
					PORTA.OUTCLR = (1<<PIN_REDLED);

					sleepCounter = 0;
					mState = SLEEP;
					PORTB_OUTCLR = (1<<BLUE_LED);
					enablePORTBInterrupt();
					sleep_mode();
				}
			}
		}else if(mState == SLEEP){
			sleep_mode();
		}else if(mState == OFF){
			
			if(buttonSensingOn == 0){	//As long as the is pressed
				
				pressType button_press = senseMagneticSwitch();
				if(button_press == VERYLONG){
					
					mState = ACTIVE;
					changeUIState(SHORT);
					changeLEDAnimation(UI_STARTUP);
				}
				_delay_ms(MAINLOOP_DELAY);
			}else{
				senseMagneticSwitch();	//Call one more time to reset the time counting variables
				sleep_mode();
			}
		}
		
	}
}
