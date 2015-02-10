/* Copyright 2014 Moreto
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
 * DHT22int.c
 * 
 * Version 1
 *
 * Main file of the DHT22 Interrupt Driven library for AVR.
 * Created: 12/01/2014 22:26:03
 * Author: Miguel Moreto
 
 * This lib can read temperature and humidity of a DHT22 sensor without
 * blocking the microcontroller with unnecessary delay functions.
 * The lib is interrupt driven, all the timing measurements of sensor
 * signal is done with a timer and a external interrupt using a state machine.
 * This way, you can use this lib with multiplexed displays without flicker the
 * display during the measurement of the sensor data.
 *
 * REQUIREMENTS: 
 *   A pin with external interrupt (INT0, INT1 or other).
 *   A timer with Clear Timer on Compare Match mode (CTC).
 *   Timer prescaler that gives a timer frequency of 1MHz.
 *
 *   Pin change interrupt is not supported yet.
 *
 * HOW IT WORKS:
 * Check the comments in this file to fully understand how it works. Basically:
 *   1) A timer in CTC is used to generate the host start condition.
 *      pin is configured as output (this is done in the timer interrupt
 *      handler function).
 *   2) Pin is switched to input with external interrupt. At each external
 *      interrupt the number of timer ticks (configured to occur at each
 *      microsecond) is counted.
 *   3) The value of the counter (microseconds) is compared with a fixed
 *      value in a state machine, this way, the signal from DHT22 is
 *      interpreted. This is done at the External Interrupt Handler.
 *
 * HOW TO USE:
 *  Include the lib:
 *
 *      #include "DHT22int.h"
 *
 *  Before entering the main loop of your program, declare some needed variables
 *  call the init function and enable interrupts.
 *
 *      DHT22_STATE_t state;
 *      DHT22_DATA_t sensor_data;
 *      DHT22_Init();
 *      sei();
 *
 *  Periodically (or not, depending of your use), call the function to 
 *  start reading the sensor:
 *
 *      state = DHT22_StartReading();
 *
 *  Check the state if you want to confirm that the state machine has started.
 *  In your main loop, check periodically when the data is available and in
 *  case of available, do something:
 *
 *      state = DHT22_CheckStatus(&sensor_data);
 *
 * 		if (state == DHT_DATA_READY){
 *	 		// Do something with the data.
 *          // sensor_data.temperature_integral
 *          // sensor_data.temperature_decimal
 *          // sensor_data.humidity_integral
 *          // sensor_data.humidity_decimal
 * 		}
 *		else if (state == DHT_ERROR_CHECKSUM){
 *	 		// Do something if there is a Checksum error
 *		}
 * 		else if (state == DHT_ERROR_NOT_RESPOND){
 *	 		// Do something if the sensor did not respond
 * 		}
 *
 *  To start a new measurement, you have to call DHT22_StartReading() again.
 *
 *  IMPORTANT: You need to modify the header (.h) file accordingly with your 
 *             microcontroller, the external interrupt used (and the pin) and
 *             the timer.
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "DHT22int.h"

/* Global variables for this file */
DHT22_STATE_t state;
uint8_t overflow_cnt = 0;
uint8_t bitcounter = 0;

uint8_t csPart1, csPart2, csPart3, csPart4;
uint16_t rawHumidity = 0;
uint16_t rawTemperature = 0;
uint8_t checkSum = 0;

/* NOTE: Check the macro definitions at the header file. */


/*
 * Timer Compare Match interrupt handler
 *
 * This handler is used to generate host start conditions (Periods P1 and P2).
 * Using a 8bit timer with prescaler such that a timer tick corresponds to 1us (freq. = 1MHz).
 */
ISR(TIMER_CTC_VECTOR){
	
	/* Using a 8bit timer maximum delay is 255us, we need at least 500us in Period P1.
	   Se, we need two timer interrupts. We check this with overflow_cnt and comparing
	   it the the define OVERFLOWS_HOST_START.
	   We make the pin = 0 at the begining of the state machine (function DHT22_StartReading) */
	if((state == DHT_HOST_START) && (overflow_cnt < (OVERFLOWS_HOST_START - 1))){
		overflow_cnt++;
	}
	/* After Period P1, we need to hold the pin high for aprox. 40us. So, we change timer compare
	   register to 40. */
	else if((state == DHT_HOST_START) && (overflow_cnt >= (OVERFLOWS_HOST_START - 1))){ // 510us have passed.
		PIN_HIGH(DHT22_PORT,DHT22_PIN); // Change pin to High for period P2.
		overflow_cnt = 0;
		state = DHT_HOST_PULLUP;
		TIMER_OCR_REGISTER = 40;
		return;
	}
	/* The Period P2 have passed. We need now to change the pin to input and wait for sensor
	   to respond. External INT is used. Sensor will respond by pulling the line down for aprox. 
	   80us. So, we can measure period P3 at the next rising edge interrupt. */
	else if (state == DHT_HOST_PULLUP){ // more 40us have passed
		TIMER_OCR_REGISTER = 255; // Change timer compare to 255, for now on, the timer interrupt should not fire.
		                          // If if fires, too much time has passed and something is wrong. We will clear
					              // the timer counter at the beggining of the external interrupt handler.
		SET_PIN_INPUT(DHT22_DDR,DHT22_PIN); // Set pin as input.
		PIN_HIGH(DHT22_PORT,DHT22_PIN); // Write 1 to enable pullup.
		EXT_INTERRUPT_DISABLE  // Disable external interrupt (in case it is already enable)
		EXT_INTERRUPT_SET_RISING_EDGE // Setting ext. int to rising edge.
		EXT_INTERRUPT_CLEAR_FLAG // Clear flag to avoid spurious firing of ext. int.
		EXT_INTERRUPT_ENABLE  // Re-enable external int.
		TIMER_COUNTER_REGISTER = 0; // Reset counter
		state = DHT_WAIT_SENSOR_RESPONSE; // Change state.
		return; // Return of the int. handler.
	}
	/* If the timer interrupt fired while not in the previous states, than too much time
	   has passed and we signal a error. */
	else{ 
		state = DHT_ERROR_NOT_RESPOND; // Change to a error state
		TIMER_STOP // Stop timer.
		EXT_INTERRUPT_DISABLE  // Disable external interrupt
		SET_PIN_OUTPUT(DHT22_DDR,DHT22_PIN); // Set pin back to output.
		PIN_HIGH(DHT22_PORT,DHT22_PIN); // Set pin high to disable DHT22.
		bitcounter = 0; // reset bit counter.
	}
}

/*
 * External interrupt handler
 * 
 * The external interrupt is used to measure the width of a pulse and change
 * the state accordingly.
 */
ISR(EXT_INTERRUPT_VECTOR){
	
	uint8_t counter_us;
	counter_us = TIMER_COUNTER_REGISTER; // Store counter value
	TIMER_COUNTER_REGISTER = 0; // Reset counter.
	
	/* Period P3. Sensor pulls down the line for aprox. 80us.
	   The ext int. was configured to rising edge. If counter is aprox. 80,
	   (or  < 100 in this case) when the line rises it
	   indicates that the sensor responded.
	   Now we have to change interrupt sense to falling edge in order to
	   detect the period P4.
	 */
	if ((state == DHT_WAIT_SENSOR_RESPONSE && (counter_us > 60) && counter_us < 100)){ // Sensor responded (Period P3).
		EXT_INTERRUPT_DISABLE  // Disabling interrupt.
		EXT_INTERRUPT_SET_FALLING_EDGE  // Changing interrupt sense to falling edge.
		EXT_INTERRUPT_CLEAR_FLAG  // clearing flag (this prevents interrupt to fire when changing to falling edge).
		EXT_INTERRUPT_ENABLE  // Re-enabling interrupt.
		state = DHT_SENSOR_PULLUP; // Changing state.
		return;
	}
	/* Period P4. When the falling edge interrupt occurs, indicating the end of P4,
	   we get the counter register and check it value. If it is less than 100 (period
	   P4 is also aprox. 80us) then the sensor responded pulling up the line. Now the 
	   bit transmission will start and we only need to measure the with of each bit. So,
	   the external interrupt can stay on falling edge. */
	else if((state == DHT_SENSOR_PULLUP) && (counter_us > 60) && (counter_us < 100)){ // Sensor responded (Period P4).
		state = DHT_TRANSFERING; // Change state
		return;
	}
	/* Period P5. Measuring the with of the pulse in order to determine if it is a 0 or a 1.
	   Bit 0 has a period of 50us + 28us. So we check if it is larger than 50us and smaller than 110. (DHT22 timing is no precise, neither the timer) */
	else if((state == DHT_TRANSFERING) && (counter_us > 50) && (counter_us <= 110)){ // Sensor sent a databit 0 (Period P5).
		// If bit is a 0, only increment the bit counter (we need only to shift 1's).
		bitcounter++; 
	}
	/* Period P5. Bit 1 has a period of 50us + 70us. So, we check if it is lager than 50us and smaller than 160. */
	else if((state == DHT_TRANSFERING) && (counter_us > 110) && (counter_us <= 160)){ // Sensor sent a databit 1 (Period P5).
		/* If bit is one, we shift one to the variables rawHumidity, rawTemperature and checkSum according
		   with bit position givem by bitcounter */
		if (bitcounter < 16) // Humidity
		{
			rawHumidity |= (1 << (15 - bitcounter));
		}
		if ((bitcounter > 15) && (bitcounter < 32))  // Temperature
		{
			rawTemperature |= (1 << (31 - bitcounter));
		}
		if ((bitcounter > 31) && (bitcounter < 40))  // CRC data
		{
			checkSum |= (1 << (39 - bitcounter));
		}
		bitcounter++; // Increments bit counter, the state does not change. 
	}
	
	/* Check if all bits arrived. If so, stop the timer and external interrupt. */
	if (bitcounter > 39){ // Transfer done
		TIMER_STOP // Stop timer.
		TIMER_COUNTER_REGISTER = 0; // Reset counter.
		EXT_INTERRUPT_DISABLE // Disabling interrupt
		SET_PIN_OUTPUT(DHT22_DDR,DHT22_PIN);
		PIN_HIGH(DHT22_PORT,DHT22_PIN);
		bitcounter = 0; // Reset bit counter.
		state = DHT_CHECK_CRC; // Change state.
	}
	
	/* CRC check is done at outside interrupt handler, by the
	function DHT22_CheckStatus. This way, this handler is very fast. */
				
}

/*
 * DHT22_STATE_t DHT22_CheckStatus(DHT22_DATA_t* data)
 *
 * Function that should be called after DHT22_StartReading() in order to check
 * if a transfer is complete.
 *
 * It returns a DHT22_STATE_t variable with the state of the state machine.
 *  Returned values:
 *    DHT_DATA_READY: Data is ok and can be used by the main program.
 *    DHT_ERROR_CHECKSUM: Error, checksum does no match.
 *    DHT_ERROR_NOT_RESPOND: Sensor is not connected or not responding for some reason.
 */

DHT22_STATE_t DHT22_CheckStatus(DHT22_DATA_t* data){
	
	/* If a transfer is complete, check CRC and update sensor data structure */
	if (state == DHT_CHECK_CRC){
		
		// calculate checksum:
		csPart1 = rawHumidity >> 8;
		csPart2 = rawHumidity & 0xFF;
		csPart3 = rawTemperature >> 8;
		csPart4 = rawTemperature & 0xFF;
		
		if( checkSum == ( (csPart1 + csPart2 + csPart3 + csPart4) & 0xFF ) ){ // Checksum correct
			
			/* raw data to sensor values */
			data->humidity_integral = (uint8_t)(rawHumidity / 10);
			data->humidity_decimal = (uint8_t)(rawHumidity % 10);			
			if(rawTemperature & 0x8000)	// Check if temperature is below zero, non standard way of encoding negative numbers!
			{
				rawTemperature &= 0x7FFF; // Remove signal bit
				data->temperature_integral = (int8_t)(rawTemperature / 10) * -1;
				data->temperature_decimal = (uint8_t)(rawTemperature % 10);
			} else
			{
				data->temperature_integral = (int8_t)(rawTemperature / 10);
				data->temperature_decimal = (uint8_t)(rawTemperature % 10);
			}
			state = DHT_DATA_READY;
		}
		else{
			state = DHT_ERROR_CHECKSUM;
		}
	}
	
	return state;
}

/*
 * void DHT22_Init(void)
 *
 * Function to be called before the main loop.
 * It configures the sensor pin and timer mode.
 */
void DHT22_Init(void){
	
	/* Configuring DHT pin as output (initially) */
	DHT22_DDR |= (1 << DHT22_PIN);
	PIN_HIGH(DHT22_PORT,DHT22_PIN);
	
	/* Timer config. */
	TIMER_SETUP_CTC  // Seting timer to CTC
	TIMER_ENABLE_CTC_INTERRUPT  // Enable compare match interrupt
	// Timer is started by the function DHT22_StartReading. For now
	// it remains with prescaler = 0 (disable).
	TIMER_STOP
	
	state = DHT_STOPPED;
	
}

/*
 * DHT22_STATE_t DHT22_StartReading(void)
 *
 * This function starts a new reading of the sensor.
 * It returns a variable of type DHT22_STATE_t with the possible values:
 *    DHT_BUSY: The reading did not started because the state machine 
 *              is doing something else, indicating that the previous
 *              reading did not finished.
 *    DHT_STARTED: The state machine has successfully started. The user
 *                 can wait for data using DHT22_CheckStatus() function.
 */
DHT22_STATE_t DHT22_StartReading(void){
	
	/* Check if the state machine is stopped. If so, start it. */
	if (state == DHT_STOPPED || state == DHT_DATA_READY || state == DHT_ERROR_CHECKSUM || state == DHT_ERROR_NOT_RESPOND){
		/* Reset values and counters */
		rawTemperature = 0;
		rawHumidity = 0;
		checkSum = 0;
		overflow_cnt = 0;
		bitcounter = 0;
		/* Configuring peripherals */
		//EIMSK &= ~(1 << INT0); // Disable external interrupt
		EXT_INTERRUPT_DISABLE
		DHT22_DDR |= (1 << DHT22_PIN); // Configuring sensor pin as output.
		PIN_LOW(DHT22_PORT,DHT22_PIN); // Write 0 to pin. Start condition sent to sensor.
		TIMER_OCR_REGISTER = 255; // Timer compare value equals to overflow (interrupt will fired at 255us).
		TIMER_COUNTER_REGISTER = 0; // Reset counter value.
		state = DHT_HOST_START; // Change state.
		TIMER_START // Start timer with prescaler such that 1 tick equals 1us (freq = 1MHz).
		return DHT_STARTED; // Return value indicating that the state machine started.
	}
	else{
		return DHT_BUSY; // If state machine is busy, return this value.
	}
	
} // end DHT22_StartReading