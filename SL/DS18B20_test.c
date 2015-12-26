/*
 * DS18B20_test.c
 *
 * Created: 15.12.2015
 *  Author: Steffen Loeffler (based on work from Irfan Sehic)
 */ 

#include "ds18S20.h"
#include <stdio.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "sensorIO.h"
// #include <math.h>

 // #define F_CPU 8000000 not here, do it in project properties/compiler symbol F_CPU=8000000


static int uart_putchar(char c, FILE *stream);
uint8_t uart_getchar(void);


static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

#define FOSC 8000000
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

int retval;


avr_port t_sensor;
extern void write_sensor( uint8_t data, avr_port sensor) ;
void down_up(avr_port sensor, int b);
uint8_t wait_for_key(uint8_t silent);	
uint8_t read_bit(avr_port sensor);

int main(void)
{ 
    // sensor is  the DQ of the temperature sensor
	t_sensor.port=&PORTD;
	t_sensor.pin=&PIND;
	t_sensor.ddr=&DDRD;
	t_sensor.mask=7;
	
	 
    DDRC |= 1 <<5; // status LED is output
	DDRD &= ~ 1<<0; // RX on PD0 is input
	DDRD |=   1<<1; // TX on PD1 is output 
	
	//USART Baud rate: 9600
    UBRR0H = MYUBRR >> 8;
    UBRR0L = MYUBRR;
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    
	stdout = &mystdout; //Required for printf init

	PORTC|= 1 <<5;
	write_sensor(1,t_sensor);
	
	printf("\n\nstarting anew ...\n");
	int i;
	i = 0;
	uint8_t key_press ;
	int j = 0;
 	while (key_press != 'q' && key_press != 'd') {
		j++;
		down_up(t_sensor, 0);
		while (i<8) {
			down_up(t_sensor, !(j & 1<<i));
			i++;
		}
		printf ("loop of 10 toggles  is done\n");
		key_press = wait_for_key(0);
		i=0;	
	} /**/

	
	
	int debug = 0;
	uint8_t sp[10];
	uint8_t my_addr[8];
	uint8_t my_bit;
	uint16_t prog_cntr;
	uint8_t conv_start;
	
	

	printf("\n");
	printf("h: help\n");
	prog_cntr = 0;

	while(1)
	{
		
		key_press = wait_for_key(1);
		prog_cntr++;
		switch(key_press) {
			case 'h': 
					printf("\n");
					printf("0: write 0\n");
					printf("1: write 1\n");
					printf("b: read 1 bit\n");
					printf("a: READ_ROM (0x%02x)\n",READ_ROM);
					printf("d: toggle debug, currently debug is:%u\n",debug);
					printf("e: temperature conversion without reading the status for parasite power\n");
					printf("i: initialize (reset)\n");
					printf("h: this help\n");
					printf("k: MATCH_ROM for device 28.453506050000ea (0x%02x)\n",MATCH_ROM);
					printf("m: MATCH_ROM for device 28.494005050000a0 (0x%02x)\n",MATCH_ROM);
					printf("l: MATCH_ROM for device 28.f5c905050000a5 (0x%02x)\n",MATCH_ROM);
					printf("7: MATCH_ROM for device 28.266e050500001d (0x%02x)\n",MATCH_ROM);
					printf("8: MATCH_ROM for device 28.14b70505000033 (0x%02x)\n",MATCH_ROM);
					printf("9: MATCH_ROM for device 28.a26305050000c7 (0x%02x)\n",MATCH_ROM);
					printf("s: SKIP_ROM (0x%02x)\n",SKIP_ROM);
					printf("c: CONVERT_T (0x%02x) and wait 1ms\n",CONVERT_T);
					printf("t: READ_SCRATCHPAD (0x%02x) , READ 2 bytes, print temperature\n",READ_SCRATCHPAD);
				break;
			case 'a': 
				printf("%u: READ_ROM: 0x%02x\n",prog_cntr,READ_ROM);
				printf("reading address: ");
				transmit_data(READ_ROM,t_sensor); 
				for (i=0; i<8; i++){
					my_addr[i] = read_data(t_sensor);
				}
				for (i=0; i<8; i++){
					printf("%02x",my_addr[i]);
				}
				printf("\n");
				break;
			case '0': 
					transmit_bit(0,t_sensor);
					printf("0");
				break;
			case '1':
					transmit_bit(1,t_sensor);
					printf("1");
				break;
			case 'n':
					printf("\n");
				break;
			case ' ':
					printf(" ");
				break;
			case (char) 13:
					printf("\n");
				break;
			case 'B':
				printf(" 0x%02x",read_data(t_sensor));
				break;
			case 'b':
				printf("%x",read_bit(t_sensor));
				break;
			case 'c':
				printf("%u: CONVERT_T: 0x%02x\n",prog_cntr,CONVERT_T);
				conv_start = 0;
				transmit_data(CONVERT_T,t_sensor); // Tell the sensor to prepare the data, 0x44 = 68
				
				if (!read_bit(t_sensor)){conv_start = 1;}
				while (!read_bit(t_sensor)){conv_start++;}
				
				if (conv_start){ printf("The temperature conversion completed after %u loops.\n",conv_start);}
				else {printf("The temperature conversion never started!\n");}
				
				break;
			case 'e':
				printf("%u: CONVERT_T: 0x%02x without reading status (good for parasite power)\n",prog_cntr,CONVERT_T);
				conv_start = 0;
				transmit_data(CONVERT_T,t_sensor); // Tell the sensor to prepare the data, 0x44 = 68
				//sbi(*t_sensor.port, t_sensor.mask); // drive high
				//sbi(*t_sensor.ddr,t_sensor.mask);    // output
				printf("the conversion was started.\n");
				
				//if (!read_bit(t_sensor)){conv_start = 1;}
				//while (!read_bit(t_sensor)){conv_start++;}
				
				//if (conv_start){ printf("The temperature conversion completed after %u loops.\n",conv_start);}
				//else {printf("The temperature conversion never started!\n");}
				_delay_ms(1000);
				printf("the conversion SHOULD be done now.\n");
				break;
			case 'i':
				printf("%u: reset/presence detect\n",prog_cntr);
				if(sensor_init(t_sensor))
				{	
					printf("     -> sensor init failed\n");
				}
				else
				{
 					if (debug) {
						printf("    -> sensor init succeeded\n");
					}
				}	
				break;
			case 'd': 
					debug = !debug;
					printf("debug = %u\n",debug);
					break;
			case 's': 
					printf("%u: SKIP_ROM: 0x%02x\n",prog_cntr, SKIP_ROM);
					transmit_data(SKIP_ROM,t_sensor);
					break;
			case '7':
					printf("%u: MATCH_ROM device 28.266e050500001d : 0x%02x\n",prog_cntr, MATCH_ROM);
					transmit_data(MATCH_ROM,t_sensor);
					transmit_data(0x28,t_sensor);
					transmit_data(0x26,t_sensor);
					transmit_data(0x6e,t_sensor);
					transmit_data(0x05,t_sensor);
					transmit_data(0x05,t_sensor);
					transmit_data(0x00,t_sensor);
					transmit_data(0x00,t_sensor);
					transmit_data(0x1d,t_sensor);
					printf("the ROM data has been submitted\n");
					break;
			case '8':
					printf("%u: MATCH_ROM device 28.14b70505000033 : 0x%02x\n",prog_cntr, MATCH_ROM);
					transmit_data(MATCH_ROM,t_sensor);
					transmit_data(0x28,t_sensor);
					transmit_data(0x14,t_sensor);
					transmit_data(0xb7,t_sensor);
					transmit_data(0x05,t_sensor);
					transmit_data(0x05,t_sensor);
					transmit_data(0x00,t_sensor);
					transmit_data(0x00,t_sensor);
					transmit_data(0x33,t_sensor);
					printf("the ROM data has been submitted\n");
					break;
			case '9':
					printf("%u: MATCH_ROM device 28.a26305050000c7 : 0x%02x\n",prog_cntr, MATCH_ROM);
					transmit_data(MATCH_ROM,t_sensor);
					transmit_data(0x28,t_sensor);
					transmit_data(0xa2,t_sensor);
					transmit_data(0x63,t_sensor);
					transmit_data(0x05,t_sensor);
					transmit_data(0x05,t_sensor);
					transmit_data(0x00,t_sensor);
					transmit_data(0x00,t_sensor);
					transmit_data(0xc7,t_sensor);
					printf("the ROM data has been submitted\n");
					break;
			case 'm':
					printf("%u: MATCH_ROM device 28.4940050500 : 0x%02x\n",prog_cntr, MATCH_ROM);
					transmit_data(MATCH_ROM,t_sensor);
					transmit_data(0x28,t_sensor);
					transmit_data(0x49,t_sensor);
					transmit_data(0x40,t_sensor);
					transmit_data(0x05,t_sensor);
					transmit_data(0x05,t_sensor);
					transmit_data(0x00,t_sensor);
					transmit_data(0x00,t_sensor);
					transmit_data(0xa0,t_sensor);
					printf("the ROM data has been submitted\n");
					break;
			case 'l':
					printf("%u: MATCH_ROM device 28.f59c050500a5 : 0x%02x\n",prog_cntr, MATCH_ROM);
					transmit_data(MATCH_ROM,t_sensor);
					transmit_data(0x28,t_sensor);
					transmit_data(0xf5,t_sensor);
					transmit_data(0x9c,t_sensor);
					transmit_data(0x05,t_sensor);
					transmit_data(0x05,t_sensor);
					transmit_data(0x00,t_sensor);
					transmit_data(0x00,t_sensor);
					transmit_data(0xa5,t_sensor);
					printf("the ROM data has been submitted\n");
					break;
			case 'k':
					printf("%u: MATCH_ROM device 28.453506050000ea : 0x%02x\n",prog_cntr, MATCH_ROM);
					transmit_data(MATCH_ROM,t_sensor);
					transmit_data(0x28,t_sensor);
					transmit_data(0x45,t_sensor);
					transmit_data(0x35,t_sensor);
					transmit_data(0x06,t_sensor);
					transmit_data(0x05,t_sensor);
					transmit_data(0x00,t_sensor);
					transmit_data(0x00,t_sensor);
					transmit_data(0xea,t_sensor);
					printf("the ROM data has been submitted\n");
					break;
			case 'q':
				printf("SEARCH_ROM: 0x%02x\n",SEARCH_ROM);
				transmit_data(SEARCH_ROM,t_sensor);
				break;
			case 't':
				printf("%u: READ_SCRATCHPAD: 0x%02x\n",prog_cntr,READ_SCRATCHPAD);
				transmit_data(READ_SCRATCHPAD,t_sensor);
				printf("reading scratchpad: ");
				for (i=0; i<9; i++){
					sp[i] = read_data(t_sensor);
				}
				for (i=0; i<9; i++){
					printf("%02x",sp[i]);
				}
				printf("\n");
				printf("temperature: %2u.%3u C\n"
					,(uint16_t)((sp[1])<<8 | sp[0])>>4
					,(uint16_t)( 0x271 * (sp[0] & 0x0f) )
					);
				printf("Th: 0x%02x\n",sp[2]);
				printf("Tl: 0x%02x\n",sp[3]);
				printf("config : ");
				for (i=0; i<8; i++){
					printf("%x",(sp[4]& 0x80)>>7);
					sp[4] = sp[4]<<1;
				}
				printf("\n");
				printf("ERC: 0x%02x\n",sp[8]);
				// printf("temperature 0x%04x = %6.3f C\n",(((0x7&sp[1])<<8)+sp[0]),(((0x7&sp[1])<<8)+sp[0])*0.0625);
/*				break;
				uint8_t temp_lower = read_data(t_sensor);
				uint8_t temp_higher = read_data(t_sensor);
				uint16_t temp  = ((0x7 & temp_higher) << 8) + temp_lower;
				printf("temp=%6.3f\n\n",temp*0.0625);
				/**/
				break;
		}
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


void down_up(avr_port sensor, int b){
	//PORTC &= ~ (1<<5);
	write_sensor(b,sensor);
	_delay_us(1);
	//PORTC|= 1 <<5;
	write_sensor(1, sensor);
	_delay_us(1);
}			



uint8_t read_bit(avr_port sensor){
	uint8_t bit;
	
	_delay_us(1);
	write_sensor(0,sensor); // pull bus down. initiate transfer
	_delay_us(2);
	write_sensor(1,sensor); // release bus,
	_delay_us(1);			// now read status
	
	// 2: orange (channel 2: latch timing)
	// 3: yellow (channel 3): sensed data
	DDRD |= 1<<2 | 1<<3;
	bit = *sensor.pin & (1<< sensor.mask);
	PORTD |= 1<<2 ; //rising edge shows where I latch data
	if(bit)
	{
		bit = 1;
		PORTD |= 1<<3 ;
	}
	else
	{
		bit = 0;
	}
	_delay_us(1);		
	PORTD &= ~(1<<3);
	return (bit);
}
		
		