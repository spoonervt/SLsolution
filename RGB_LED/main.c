/*
 * RGB_LED.c
 *
 * Created: 12/16/2015 12:56:52 PM
 * Author : steffen
 */ 


#define __SFR_OFFSET 0 
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

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

typedef uint8_t   u8;
typedef uint16_t  u16;

#define NUM_WS2812    6
#define NUM_LEDS      (NUM_WS2812*3)

enum {S_R, S_G, S_B, S_Y, S_V, S_T};

#define MAX   50

#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

// declaration of our ASM function
extern void output_grb(u8 * ptr, u16 count);

void set_color(u8 * p_buf, u8 led, u8 r, u8 g, u8 b)
{
	u16 index = 3*led;
	p_buf[index++] = g;
	p_buf[index++] = r;
	p_buf[index] = b;
}

int main(void)
{
	u8 buf[NUM_LEDS];
	int count = 0;

	DDRD |= 1<<0;	
	DDRD |= 1<<1;	
	DDRD |= 1<<2;	
	DDRC |= 1<<0;	
	DDRC |= 1<<1;	
	DDRC |= 1<<2;	
	DDRB |= 1<<0;	
	DDRB |= 1<<1;	
	DDRB |= 1<<2;	
//	sbi(DDRB,0);
// 	sbi(DDRB,1);
//	sbi(DDRB,2);

	memset(buf, 0, sizeof(buf));

	u8 state = S_R;
	u8 val = 0;
	u8 first_time = 1;
	
	while(1)
	{
		if (PINB & (1<<2)){
			cbi(PORTB,2);
		} else {
			sbi(PORTB,2);
		}
		//output_grb(buf, sizeof(buf));
		
		
		switch (state)
		{
			case S_R:
			if (++val <= MAX)
			{
				if (!first_time)
				{
					set_color(buf, 5, val, MAX-val, MAX-val);
				}
				set_color(buf, 0, val, 0, 0);
			}
			else
			{
				first_time = 0;
				state = S_G;
				val = 0;
			}
			break;
			
			case S_G:
			if (++val <= MAX)
			{
				set_color(buf, 0, MAX-val, val, 0);
				set_color(buf, 1, 0, val, 0);
			}
			else
			{
				state = S_B;
				val = 0;
			}
			break;
			
			case S_B:
			if (++val <= MAX)
			{
				set_color(buf, 1, 0, MAX-val, val);
				set_color(buf, 2, 0, 0, val);
			}
			else
			{
				state = S_Y;
				val = 0;
			}
			break;
			
			case S_Y:
			if (++val <= MAX)
			{
				set_color(buf, 2, val, 0, MAX-val);
				set_color(buf, 3, val, val, 0);
			}
			else
			{
				state = S_V;
				val = 0;
			}
			break;
			
			case S_V:
			if (++val <= MAX)
			{
				set_color(buf, 3, MAX-val, MAX-val, val);
				set_color(buf, 4, val, 0, val);
			}
			else
			{
				state = S_T;
				val = 0;
			}
			break;
			
			case S_T:
			if (++val <= MAX)
			{
				set_color(buf, 4, MAX-val, val, MAX-val);
				set_color(buf, 5, 0, val, val);
			}
			else
			{
				state = S_R;
				val = 0;
			}
			break;
			
			default:
			state = S_R;
			break;
		}

		_delay_ms(100);
	}
}
