/*
 * togglePortB.c
 *
 * Created: 12/16/2015 9:56:23 PM
 * Author : steffen
 */ 

#include <avr/io.h>


int main(void)
{
    /* Replace with your application code */
	DDRB = 0xff;
    while (1) 
    {
		if (PINB & 1<<1){
			PORTB &= ~(1<<1);
		} else {
		}
			PORTB |= (1<<1);
    }
}

