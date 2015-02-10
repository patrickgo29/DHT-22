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
/* DHT22.h
 *
 * DHT22 Driver library for AVR
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
 * 
 * Miguel Moreto, Brazil, 2013.
 */
#define F_CPU 16000000UL

#ifndef _DHT22_H_
#define _DHT22_H_

#include <util/delay.h>
#include <avr/io.h>

/* 
Output format setting. Change to 1 to output a struct with
the raw values from DHT22. If decimal and integral parts of
the values are required leave equal to 0.
*/
#define OUTPUT_RAW_VALUES 0


#define DHT22_DATA_BIT_COUNT 40

/* Configure port and pin */
#define DHT22_PIN PD6
#define DHT22_DDR DDRD
#define DHT22_PORT_OUT PORTD
#define DHT22_PORT_IN PIND

typedef enum
{
  DHT_ERROR_NONE = 0,
  DHT_BUS_HUNG = 1,
  DHT_ERROR_NOT_PRESENT = 2,
  DHT_ERROR_ACK_TOO_LONG = 3,
  DHT_ERROR_SYNC_TIMEOUT = 4,
  DHT_ERROR_DATA_TIMEOUT = 5,
  DHT_ERROR_CHECKSUM = 6,
} DHT22_ERROR_t;

// Output format structure.
#if(OUTPUT_RAW_VALUES==0)
typedef struct
{
	int8_t temperature_integral;
	uint8_t temperature_decimal;
	uint8_t humidity_integral;
	uint8_t humidity_decimal;
	uint8_t retryCount;
} DHT22_DATA_t;
#else
typedef struct
{
	int16_t raw_temperature;
	uint16_t raw_humidity;
} DHT22_DATA_t;
#endif

DHT22_ERROR_t readDHT22(DHT22_DATA_t* data);


#endif
