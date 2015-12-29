/*
 * RomSearch.c
 *
 * Created: 12/18/2015 10:27:09 PM
 * Author : steffen
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "serialCommunication.h"
#include "avrPort.h"
#include "owCommands.h"
#include <util/delay.h>

int i;
int main(void)
{
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	uart_init(); // from serialCommunication, starts UART and redirects STDOUT/IN
	
	avrPort * owChannel;
	avrPort Channel1;
	owChannel = &Channel1;
	owChannel->port=&PORTD;
	owChannel->pin=&PIND;
	owChannel->ddr=&DDRD;
	owChannel->mask=7;	
	
	int rc;
	int key_press;
	int conv_start = 0;
	int debug = 0;
	uint8_t my_addr[8];
	int prog_cntr = 0;
	uint8_t sp[10];
	
	printf("The program was restarted ...\n\n");
	
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
			case '0': 
					transmit_bit(0,owChannel);
					printf("0");
				break;
			case '1':
					transmit_bit(1,owChannel);
					printf("1");
				break;
			case ' ':
				printf(" ");
				break;
			case '/':
				printf("MATCH_ROM, selecting : %0x.%02x%02x%02x%02x%02x%02x%02x\n"
					,my_addr[0]
					,my_addr[1]
					,my_addr[2]
					,my_addr[3]
					,my_addr[4]
					,my_addr[5]
					,my_addr[6]
					,my_addr[7]
					);
					transmit_data(0x55,owChannel); 
					transmit_data(my_addr[0],owChannel); 
					transmit_data(my_addr[1],owChannel); 
					transmit_data(my_addr[2],owChannel); 
					transmit_data(my_addr[3],owChannel); 
					transmit_data(my_addr[4],owChannel); 
					transmit_data(my_addr[5],owChannel); 
					transmit_data(my_addr[6],owChannel); 
					transmit_data(my_addr[7],owChannel); 

				break;
			case 'a': 
				printf("%u: READ_ROM: 0x%02x\n",prog_cntr,READ_ROM);
				printf("reading address: ");
				transmit_data(READ_ROM,owChannel); 
				for (i=0; i<8; i++){
					my_addr[i] = read_data(owChannel);
				}
				for (i=0; i<8; i++){
					printf("%02x",my_addr[i]);
				}
				printf("\n");
				break;
			case (char) 13:
				printf("\n");
				break;
			case 'B':
				printf(" 0x%02x",read_data(owChannel));
				break;
			case 'b':
				printf("%x",read_bit(owChannel));
				break;
			case 'c':
				printf("%u: CONVERT_T: 0x%02x\n",prog_cntr,CONVERT_T);
				conv_start = 0;
				transmit_data(CONVERT_T,owChannel); // Tell the sensor to prepare the data, 0x44 = 68
				
				if (!read_bit(owChannel)){conv_start = 1;}
				while (!read_bit(owChannel)){conv_start++;}
				
				if (conv_start){ printf("The temperature conversion completed after %u loops.\n",conv_start);}
				else {printf("The temperature conversion never started!\n");}
				
				break;
			case 'd':
				debug = !debug;
				printf("debug = %u\n",debug);
				break;
			case 'e':
				printf("%u: CONVERT_T: 0x%02x without reading status (good for parasite power)\n",prog_cntr,CONVERT_T);
				conv_start = 0;
				transmit_data(CONVERT_T,owChannel); // Tell the sensor to prepare the data, 0x44 = 68
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
				if(sensor_init(owChannel))
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
			case 'n':
				printf("\n");
				break;
			case 's':
				printf("%u: SKIP_ROM: 0x%02x\n",prog_cntr, SKIP_ROM);
				transmit_data(SKIP_ROM,owChannel);
				break;
			case 't':
				printf("%u: READ_SCRATCHPAD: 0x%02x\n",prog_cntr,READ_SCRATCHPAD);
				transmit_data(READ_SCRATCHPAD,owChannel);
				printf("reading scratchpad: ");
				for (i=0; i<9; i++){
					sp[i] = read_data(owChannel);
				}
				for (i=0; i<9; i++){
					printf("%02x",sp[i]);
				}
				printf("\n");
				printf("temperature: %2u.%3u C\n"
					,(uint16_t)((sp[1])<<8 | sp[0])>>4
					,(uint16_t)( 0x271 * (sp[0] & 0x0f) )
					);
				printf("Th: %3u C\n",sp[2]);
				printf("Tl: %3u C\n",sp[3]);
				printf("config : ");
				for (i=0; i<8; i++){
					printf("%x",(sp[4]& 0x80)>>7);
					sp[4] = sp[4]<<1;
				}
				printf("\n");
				printf("ERC: 0x%02x\n",sp[8]);
				break;
		case '.':
				printf("%u: send READ_SCRATCHPAD: 0x%02x\n",prog_cntr,READ_SCRATCHPAD);
				transmit_data(READ_SCRATCHPAD,owChannel);
				break;
				}
	 }		
}
