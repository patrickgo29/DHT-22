#ifndef UARTInit
#define UARTInit

#define EVEN 0
#define ODD 1
#include <math.h>
#include <avr/io.h>

unsigned char ReceiveUART0(void)
{
	while (! (UCSR0A & (1 << RXC0)) );
	return = UDR0;
}

void TransmitUART0 (unsigned char data)
{
	//Wait until the Transmitter is ready
	while (! (UCSR0A & (1 << UDRE0)) );

	//Get that data outa here!
	UDR0 = data;
}

void InitializeUART0(int baud, char AsyncDoubleSpeed, char DataSizeInBits, char ParityEvenorODD, char StopBits)
{
	uint16_t UBBRValue = lrint(( F_CPU / (16L * baud) ) - 1);
	
	if (AsyncDoubleSpeed == 1) UCSR0A = (1 << U2X0); //setting the U2X bit to 1 for double speed asynchronous

	//Put the upper part of the baud number here (bits 8 to 11)
	UBRR0H = (unsigned char) (UBBRValue >> 8);

	//Put the remaining part of the baud number here
	UBRR0L = (unsigned char) UBBRValue;

	//Enable the receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	//Set 2 stop bits and data bit length is 8-bit
	if (StopBits == 2) UCSR0C = (1 << USBS0);
	
	if (ParityEvenorODD == EVEN) UCSRC |= (1 << UPM1); //Sets parity to EVEN
	if (ParityEvenorODD == ODD) UCSRC |= (3 << UPM0); //Alternative way to set parity to ODD
	
	if (DataSizeInBits == 6) UCSR0C |= (1 << UCSZ10); //6-bit data length
	if (DataSizeInBits == 7) UCSR0C |= (2 << UCSZ00); //7-bit data length
	if (DataSizeInBits == 8) UCSR0C |= (3 << UCSZ00); //8-bit data length
	if (DataSizeInBits == 9) UCSR0C |= (7 << UCSZ00); //9-bit data length
	
}

void InitializeUART1(int baud, char AsyncDoubleSpeed, char DataSizeInBits, char ParityEvenorODD, char StopBits)
{
	uint16_t UBBRValue = lrint(( F_CPU / (16L * baud) ) - 1);
	
	if (AsyncDoubleSpeed == 1) UCSR0A = (1 << U2X0); //setting the U2X bit to 1 for double speed asynchronous

	//Put the upper part of the baud number here (bits 8 to 11)
	UBRR1H = (unsigned char) (UBBRValue >> 8);

	//Put the remaining part of the baud number here
	UBRR1L = (unsigned char) UBBRValue;

	//Enable the receiver and transmitter
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);

	//Set 2 stop bits and data bit length is 8-bit
	if (StopBits == 2) UCSR1C = (1 << USBS1);
	
	if (ParityEvenorODD == EVEN) UCSRC |= (1 << UP11); //Sets parity to EVEN
	if (ParityEvenorODD == ODD) UCSRC |= (3 << UPM1); //Alternative way to set parity to ODD
	
	if (DataSizeInBits == 6) UCSR1C |= (1 << UCSZ10); //6-bit data length
	if (DataSizeInBits == 7) UCSR1C |= (2 << UCSZ10); //7-bit data length
	if (DataSizeInBits == 8) UCSR1C |= (3 << UCSZ10); //8-bit data length
	if (DataSizeInBits == 9) UCSR1C |= (7 << UCSZ10); //9-bit data length
	
}

#endif