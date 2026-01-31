/*
 * valve.h
 *
 * Created: 02.11.2022 16:19:30
 *  Author: SteinerF
 *
 *	Interface to the motor for opening and closing the valve
 *  Peripherals needed: Button Interrupt, ADC
 */ 


#ifndef VALVE_H_
#define VALVE_H_

//#define OPEN_CURRENT 0.45  //--> Motor got destroyed when opening really fast with the new pcb version where there is an additional 47uF cap
//#define OPEN_CURRENT 0.32	// Tried for new PCB version with cap, but seems to be to low cause there are cases where it doesnt open completely (after the motor was resting without moving)
#define OPEN_CURRENT 0.45
#define CLOSE_CURRENT 0.80
//#define SAMPLE_ACCUM ADC_SAMPNUM_ACC16_gc

#define SAMPLE_ACCUM_OPEN ADC_SAMPNUM_ACC64_gc
#define SAMPLE_ACCUM_CLOSE ADC_SAMPNUM_ACC64_gc


#define OPEN_TIMEOUT 1000		//30000
#define CLOSE_TIMEOUT 1000		//1000

#define PIN_MOTORPLUS 1		//PORTB
#define PIN_MOTORMINUS 6	//PORTA
#define PIN_MOTORSTOP 5		//PORTA

#define MAX_VOL 9.9
#define MIN_VOL 4.0
#define MIN_VOL_BEFORE_DRIVE 7.0
#define MAX_CUR 1.0			//Bei 0.2 Ohm Shunt

#define OPEN_CURRENT_LIMIT_ADC (uint16_t)(RES_10BIT / MAX_CUR * OPEN_CURRENT)
#define CLOSE_CURRENT_LIMIT_ADC (uint16_t)(RES_10BIT / MAX_CUR * CLOSE_CURRENT)
#define MIN_VOLT_ADC (uint16_t)(RES_10BIT / MAX_VOL * MIN_VOL)
#define MIN_VOLT_BEFORE_DRIVE_ADC (uint16_t)(RES_10BIT / MAX_VOL * MIN_VOL_BEFORE_DRIVE)

typedef enum {UNDEFINED=1, OPEN=2, CLOSED=5, OPENING = 20, CLOSING = 40}  valveState;
typedef enum {WRONG_SENSOR_PLACEMENT = 1, LOW_VOLTAGE=2 , HIGH_CURRENT=3, VALVE_TIMEOUT=4, NO_ERROR}  valveError;

/*
Errors Meanings
	WRONG_SENSOR_PLACEMENT: When Valve is open for longer then 2 hours, it is assumed that either the sensor is placed wrong or there is something wrong with the water source or pipes/hoses - this is the only error that can be resolves by just pressing the button
	VOLTAGELOW:		When Voltage got too low (<4V when driving, <7V before driving). Usually Valve ends up in a safe closed position
	HIGH_CURRENT:	When Current got too high when closing (the motor could potentially damage itself without this). Causes: End Stop Button is damaged or out of physical Range (Housing Deformation) OR motor gears are broken OR problems with motor brushes or coils. Valve might end up in an open or partly open position.
	VALVE_TIMEOUT:	When the Motor was driving a certain time and the valve is still not closed. Causes: Damaged Motor Gears, solder error on PCB, disconnected cables to the motor, malfunctioning motor driver IC, motor driver IC in sleep mode. Also occurs while assembling when motor shaft must be turned in a suitable position to attach the valve parts. Valve is probably in an open or partly open position.
*/

//Figure out state of valve, if not closed, close it
void initializeValve();

void openValve();

void closeValve();

void changeMotorState();

valveState getValveState();

valveError getValveError();
void setValveError(valveError err);

void motorPositioningMode();



#endif /* VALVE_H_ */