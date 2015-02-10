/* Copyright 2013 Moreto
 *
 * This file is part of DHT22 Driver library for AVR.
 *
 * DHT22 Driver library for AVR is free software: you can redistribute 
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * DHT22 Driver library for AVR is distributed in the hope that it will 
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * 
 * Please consult the GNU General Public License at http://www.gnu.org/licenses/.
 */

/* DHT22.c
 *
 * DHT22 Driver library for AVR.
 *
 * This lib is blocking, it not uses interrupts.
 * The processor keeps busy while reading sensor values.
 * The reading process can take almost 6ms (in the worst case).
 * Is this is too much for you, have a look at my 
 * interrupt driven DHT22 lib:
 * http://moretosprojects.blogspot.com.br/2014/01/dht22-interrupt-driven-library-for-avr.html
 *
 * Code by funkytransistor published at AVR Freaks forum:
 * http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=974797
 *
 * Modified in order to return four values:
 *   Integer part or temperature
 *   Decimal part of temperature
 *   Integer part of humidity
 *   Decimal part of humidity
 * The lib as also rewrited to reduce code. Using float values the AVR-GCC 
 * included some huge library
 * The code was more than 2kB. Now it uses only 298bytes and it is
 * more efficient.
 *
 * Miguel Moreto, Brazil, 2013.
 *
 * USAGE:
 *
 * Please, disable interrupts while reading sensor. Interrupting
 * the reading process can lead to wrong measurements and CRC error.
 *
 * Example of use:
 *
 * DHT22_DATA_t sensor_values;
 * DHT22_ERROR_t error;
 * cli();
 * error = readDHT22(&sensor_values);
 * sei();
 *
 * Remember to configure the pin where the sensor is connected
 * at the header file (DHT22.h).
 */
#include "DHT22.h"

DHT22_ERROR_t readDHT22(DHT22_DATA_t* data)
{

	uint8_t retryCount = 0;
	uint8_t csPart1, csPart2, csPart3, csPart4;
	uint16_t rawHumidity = 0;
	uint16_t rawTemperature = 0;
	uint8_t checkSum = 0;
	uint8_t i;

	// Pin needs to start HIGH, wait until it is HIGH with a timeout
	retryCount = 0;
//	cli();
	DHT22_DDR &= ~(1 << ( DHT22_PIN ));
//	sei();
	do
	{
		if(retryCount > 125) return DHT_BUS_HUNG;
		retryCount++;
		_delay_us(2);
	} while( !( DHT22_PORT_IN & ( 1 << DHT22_PIN ) ) );				//!DIRECT_READ(reg, bitmask)

	
	// Send the activate pulse
//	cli();
	DHT22_PORT_OUT &= ~(1 << ( DHT22_PIN )); 							//DIRECT_WRITE_LOW(reg, bitmask);
	DHT22_DDR |= 1 << ( DHT22_PIN );								//DIRECT_MODE_OUTPUT(reg, bitmask); // Output Low
//	sei();
	_delay_ms(2); 										// spec is 1 to 10ms
//	cli();
	DHT22_DDR &= ~(1 << ( DHT22_PIN ));							// Switch back to input so pin can float
	DHT22_PORT_OUT |= (1 << ( DHT22_PIN )); // Enable pullup.
//	sei();

	// Find the start of the ACK signal
	retryCount = 0;
	do
	{
		if (retryCount > 25) 							//(Spec is 20 to 40 us, 25*2 == 50 us)
		{
//			data->retryCount = retryCount;
			return DHT_ERROR_NOT_PRESENT;
		}
		retryCount++;
		_delay_us(2);
	} while( DHT22_PORT_IN & ( 1 << DHT22_PIN ) ); // While pin is 1.
	// Aqui retrayCount foi 8 = 16us.
	
	// Here sensor responded pulling the line down DHT22_PIN = 0

	// Find the transition of the ACK signal
	retryCount = 0;
	do
	{
		if (retryCount > 50) 							//(Spec is 80 us, 50*2 == 100 us)
		{
			//data->retryCount = retryCount;
			return DHT_ERROR_ACK_TOO_LONG;
		}
		retryCount++;
		_delay_us(2);
	} while( !(DHT22_PORT_IN & ( 1 << DHT22_PIN )) );
	// Aqui retryCount foi 27 = 54us.
		
	// Here sensor pulled up DHT22_PIN = 1

	// Find the end of the ACK signal
	retryCount = 0;
	do
	{
		if (retryCount > 50) 							//(Spec is 80 us, 50*2 == 100 us)
		{
			return DHT_ERROR_ACK_TOO_LONG;
		}
		retryCount++;
		_delay_us(2);
	} while( DHT22_PORT_IN & ( 1 << DHT22_PIN ) );
	// Aqui retryCount foi 28 = 56us.
	
	
	// Here sensor pulled down to start transmitting bits.

	// Read the 40 bit data stream
	for(i = 0; i < DHT22_DATA_BIT_COUNT; i++)
	{
		// Find the start of the sync pulse
		retryCount = 0;
		do
		{
			if (retryCount > 35) 						//(Spec is 50 us, 35*2 == 70 us)
			{
				return DHT_ERROR_SYNC_TIMEOUT;
			}
			retryCount++;
			_delay_us(2);
		} while( !(DHT22_PORT_IN & ( 1 << DHT22_PIN )) );

		// No primeiro bit, retrayCount foi 18 = 36us.
//		if (i == 0){
//			data->retryCount = retryCount;	
//		}
		
		// Measure the width of the data pulse
		retryCount = 0;
		do
		{
			if (retryCount > 50) 						//(Spec is 80 us, 50*2 == 100 us)
			{
				return DHT_ERROR_DATA_TIMEOUT;
			}
			retryCount++;
			_delay_us(2);
		} while( DHT22_PORT_IN & ( 1 << DHT22_PIN ) );

		// Identification of bit values.
		if (retryCount > 20) // Bit is 1: 20*2 = 40us (specification for bit 0 is 26 a 28us).
		{
			if (i < 16) // Humidity 
			{
				rawHumidity |= (1 << (15 - i));
			}
			if ((i > 15) && (i < 32))  // Temperature
			{
				rawTemperature |= (1 << (31 - i));
			}
			if ((i > 31) && (i < 40))  // CRC data
			{
				checkSum |= (1 << (39 - i));
			}
		}
	}

	// translate bitTimes
	// 26~28us == logical 0
	// 70us	   == logical 1
	// here threshold is 40us

	// calculate checksum
	csPart1 = rawHumidity >> 8;
	csPart2 = rawHumidity & 0xFF;
	csPart3 = rawTemperature >> 8;
	csPart4 = rawTemperature & 0xFF;
	
	if( checkSum == ( (csPart1 + csPart2 + csPart3 + csPart4) & 0xFF ) )
	{
#if(OUTPUT_RAW_VALUES==0)
		// raw data to sensor values
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
#else
		if(rawTemperature & 0x8000)	// Check if temperature is below zero, non standard way of encoding negative numbers!
		{
			rawTemperature &= 0x7FFF; // Remove signal bit
			data->raw_temperature = ((int16_t)rawTemperature) * -1;
		} else
		{
			data->raw_temperature  = rawTemperature;
		}	
		data->raw_humidity = rawHumidity;
#endif
		return DHT_ERROR_NONE;
	}
	return DHT_ERROR_CHECKSUM;
}




