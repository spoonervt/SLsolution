/*
 * serialCommunication.c
 *
 * Created: 12/20/2015 8:36:59 PM
 *  Author: steffen
 *
 * includes sub for serialCommunication
 */ 

#include <avr/io.h>
#include <stdio.h>

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

static int uart_putchar(char c, FILE *stream)
{
	if (c == '\n') uart_putchar('\r', stream);
	
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	
	return 0;
}
uint8_t uart_getchar(void)
{
	// this is the original
	// it wait for a key to be pressed and
	// then returns it
	//    while( !(UCSR0A & (1<<RXC0)) );
	//    return(UDR0);

	// this here just checks if a key has been pressed
	// and returns it, otherwise 0
	if ( !(UCSR0A & (1<<RXC0)) ) {
	return(0);}
	else { return(UDR0);}
}
