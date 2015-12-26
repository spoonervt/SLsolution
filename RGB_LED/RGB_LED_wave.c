/*
 * RGB_LED_wave.c
 *
 * Created: 12/16/2015 12:56:52 PM
 * Author : steffen
 */ 


// #define __SFR_OFFSET 0 
#include <avr/io.h> 


/*xtern void output_grb(u8 * ptr, u16 count)
;
; r18 = data byte
; r19 = 7-bit count
; r20 = 1 output
; r21 = 0 output
; r22 = SREG save
; r24:25 = 16-bit count
; r26:27 (X) = data pointer
*/

 //based on http://www.embeddedrelated.com/showarticle/528.php
 
#define F_CPU   8000000

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>


typedef uint8_t   u8;
typedef uint16_t  u16;

#define NUM_WS2812    6
#define NUM_LEDS      (NUM_WS2812*3)

enum {S_R, S_G, S_B, S_Y, S_V, S_T};

#define MAX   50


// declaration of our ASM function
extern void output_grb(u8 * ptr, u16 count);

void set_color(u8 * p_buf, u8 led, u8 r, u8 g, u8 b)
{
	u16 index = 3*led;
	p_buf[index++] = g;
	p_buf[index++] = r;
	p_buf[index] = b;
}
/**/
int main(void)
{
	DDRB |= 1<<0;
	DDRB |= 1<<1;
	DDRB |= 1<<2;

	u8 buf[NUM_LEDS];
	int count = 0;
	
	memset(buf, 0, sizeof(buf));

	u8 state = S_R;
	u8 val = 0;
	u8 ws2812;
	u8 r = 0;
	u8 g = 0;
	u8 b = 0;
	u8 pos = 0;
	
	while(1)
	{
	/*
	if (PINB & (1<<1)){
		PORTB &= ~(1<<1);
	} else {
		PORTB |= (1<<1);
	}
	/**/
	output_grb(buf, sizeof(buf));
	
	for (r= 0; r<50; r++){
		for (g = 0; g<50; g++){
			for (b = 0; b<50; b++){
				set_color(buf, pos, r, g, b);
				if (pos==0){set_color(buf, NUM_WS2812 - 1 , 0, 0, 0);}
				else {set_color(buf, pos -1, 0, 0, 0);}
				}
			}
		}
	}
		
	pos++;
	if (pos== NUM_WS2812){pos=0;}
	_delay_ms(10);
	
}
