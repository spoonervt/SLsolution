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
#include "serialCommunication.h"

static int uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

#define FOSC 8000000
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

void uart_init(){
	 	//USART Baud rate: 9600
	 	UBRR0H = MYUBRR >> 8;
	 	UBRR0L = MYUBRR;
	 	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	 	stdout = &mystdout; //Required for printf init
}
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
uint8_t wait_for_key(uint8_t silent)
{
	uint8_t key_press = 0;
	// iif (!silent) {printf("\nprint any key to continue >");}
	while (key_press == 0) {key_press = uart_getchar();};
	/*
	switch(key_press){
		case '0': case '1': case 'b': break;
		default: 
			printf("%c< key pressed, moving on\n", (char)key_press);
			break;
	}
	*/
	return (key_press);
}