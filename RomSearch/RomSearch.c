/*
 * RomSearch.c
 *
 * Created: 12/18/2015 10:27:09 PM
 * Author : steffen
 */ 

#include <avr/io.h>
#include <stdio.h>

static int uart_putchar(char c, FILE *stream);
uint8_t uart_getchar(void);

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

#define FOSC 16000000
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

union LLU{
	long long ll;
	struct  {
		long lower_l;
		long upper_l;
		};
	}  ;

union LLU address;
union LLU mask=1;
int i;
int main(void)
{
	address.ll = 0;
 	//USART Baud rate: 9600
 	UBRR0H = MYUBRR >> 8;
 	UBRR0L = MYUBRR;
 	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	stdout = &mystdout; //Required for printf init

    while (1) 
    {
		for (i=0; i<64; i++){
			address.ll |= mask<<i;
			printf("%08lx%08lx\n",address.upper_l,address.lower_l);
			address.ll &= ~(1<<i);
		}
		while(1);
	}
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
