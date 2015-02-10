#include<avr/io.h>
#include<stdio.h>
#include<avr/interrupt.h>
#include "DHT22.h"

#define F_CPU 16000000UL
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU/(USART_BAUDRATE*16UL)))-1)

void uart0_init(void);
void send_char(char c);
void send_string(char s[]);
char get_char(void);

int main(void){
	uart0_init();

	DHT22_ERROR_t error;
	DHT22_DATA_t data;
	cli();

	while(1)
	{
		error = readDHT22(&data);
		_delay_ms(400);
		if (error == DHT_ERROR_NONE){
			char str[20];
			sprintf(str,"OK,%i.%u,%u.%u\n",data.temperature_integral,data.temperature_decimal, data.humidity_integral, data.humidity_decimal);
			send_string(str);			
		}
		else{
			char err[10];
			sprintf(err,"ERROR,%i\n",error);
			send_string(err);
		}
	}
	
	sei(); // enable interrupt
	return 0;
}

char get_char(void)
{
	// wait until the port is ready to be read
	//loop_until_bit_is_set(UCSR0A,RXC0);
	while( ( UCSR0A & ( 1 << RXC0 ) ) == 0 ){}
	
	// return the byte from the serial port
	return UDR0;
}

void send_char(char c)
{
	// wait until the port is ready to be written to
	while ((UCSR0A & (1 << UDRE0)) == 0) {};
	
	// write the byte to the serial port
	UDR0 = c;
}

void send_string(char s[])
{
   int i = 0;
   
   // send every char in the string
   while (s[i] != 0x00)
   {
	   send_char(s[i]);
	   i++;
   }
}

void uart0_init(void)
{
	UCSR0B |= (1<<RXEN0)  | (1<<TXEN0);  // Enable RX and TX
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01); // 8-bit data
	UBRR0H  = (BAUD_PRESCALE >> 8);
	UBRR0L  = BAUD_PRESCALE;
}