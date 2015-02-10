/* Copyright 2014 Miguel Moreto
 *
 * This file is part of DHT22 Interrupt Driven library for AVR.
 *
 * DHT22 Interrupt Driven library for AVR is free software: you can redistribute 
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * DHT22 Interrupt Driven library for AVR is distributed in the hope that it will 
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * 
 * Please consult the GNU General Public License at http://www.gnu.org/licenses/.
 */

/*
 * DHT22int.h
 *
 * Version 1
 *
 * Header file of the DHT22 Interrupt Driven library for AVR.
 * Created: 12/01/2014 22:25:54
 * Author: Miguel Moreto

 *
 * IMPORTANT: You need to modify this file accordingly with your microcontroller,
 *            the external interrupt used (and the pin) and the timer.
 *
 * This file is configured to the following situation:
 *    Microcontroller: ATmega328P
 *    Timer: 8bit Timer 2
 *    Pin: PD2 => INT0 pin
 *    8MHz clock from internal RC oscillator.
 *    Divide by 8 fuse not programmed (clock is not divided by 8).
 *
 * This config should also work with ATmega48A(PA), ATmega88A(PA),
 * ATmega168A(PA) and ATmega328.
 *
 * Please, see the comments at the .c file about how the lib works and how to use it.
 */


#ifndef DHT22INT_H_
#define DHT22INT_H_

/* Driver Configuration */
#define OVERFLOWS_HOST_START 2 // How many times a timer overflow is used to generate Period P1.
#define DHT22_DATA_BIT_COUNT 40 // Number of bits that the sensor send.

/* Macros: */
#define PIN_LOW(port,pin) port &= ~(1<<pin)
#define PIN_HIGH(port,pin) port |= (1<<pin)
#define SET_PIN_INPUT(portdir,pin) portdir &= ~(1<<pin)
#define SET_PIN_OUTPUT(portdir,pin) portdir |= (1<<pin)
#define PIN_TOGGLE(port,pin) port ^= (1<<pin)

/* Pin definition (change accordingly) 
   The pin must be a INT pin. Pin Change Interrupt is not sopported yet. */
#define DHT22_PIN PIND2 // INT0
#define DHT22_DDR DDRD
#define DHT22_PORT PORTD

/* User define macros. Please change this macros accordingly with the microcontroller,
   pin, the timer and also the external interrupt that you are using.
   
   IMPORTANT: You must configure the timer with a prescaler such that the tick
              is 1us, this means a timer clock freq. of 1MHz. With 8MHz clock, you
			  can set the prescaler to divide by 8. */
#define TIMER_SETUP_CTC					TCCR2A = (1 << WGM21);   // Code to configure the timer in CTC mode.
#define TIMER_ENABLE_CTC_INTERRUPT		TIMSK2 = (1 << OCIE2A);  // Code to enable Compare Match Interrupt
#define TIMER_OCR_REGISTER				OCR2A			// Timer output compare register.
#define TIMER_COUNTER_REGISTER			TCNT2			// Timer counter register
#define TIMER_START						TCCR2B = (1 << CS21); // Code to start timer with 1MHz clock
#define TIMER_STOP						TCCR2B = 0; // Code to stop the timer by writing 0 in prescaler bits.
#define EXT_INTERRUPT_DISABLE			EIMSK &= ~(1 << INT0); // Code to disable the external interrupt used.
#define EXT_INTERRUPT_ENABLE			EIMSK |= (1 << INT0);  // Code to enable the external interrupt used.
#define EXT_INTERRUPT_SET_RISING_EDGE	EICRA |= (1 << ISC01) | (1 << ISC00); // Code to set the interrupt to rising edge
#define EXT_INTERRUPT_SET_FALLING_EDGE	EICRA |= (1 << ISC01); EICRA &= ~(1 << ISC00);  // Code to set the interrupt to falling edge
#define EXT_INTERRUPT_CLEAR_FLAG		EIFR |= (1 << INTF0);  // Code to clear the external interrupt flag.

/* Interrupt vectors. Change accordingly */
#define TIMER_CTC_VECTOR				TIMER2_COMPA_vect
#define EXT_INTERRUPT_VECTOR			INT0_vect

/* Typedef of a enumeration of the possible states and error status */
typedef enum
{
	DHT_STOPPED = 0,
	DHT_HOST_START,
	DHT_HOST_PULLUP,
	DHT_WAIT_SENSOR_RESPONSE,
	DHT_SENSOR_PULLUP,
	DHT_TRANSFERING,
	DHT_CHECK_CRC,
	DHT_DATA_READY,
	DHT_ERROR_NOT_RESPOND,
	DHT_ERROR_CHECKSUM,
	DHT_BUSY,
	DHT_STARTED,
} DHT22_STATE_t;

/* Typedef of the structure that holds the sensor values */
typedef struct
{
	int8_t temperature_integral;
	uint8_t temperature_decimal;
	uint8_t humidity_integral;
	uint8_t humidity_decimal;
} DHT22_DATA_t;

/* Function prototypes */
void DHT22_Init(void);
DHT22_STATE_t DHT22_StartReading(void);
DHT22_STATE_t DHT22_CheckStatus(DHT22_DATA_t* data);


#endif /* DHT22INT_H_ */