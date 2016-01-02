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
#include "sbicbi.h"
#include <util/delay.h>
#include <avr/pgmspace.h>

uint8_t search_rom(uint8_t addr_str[64], avrPort * owChannel);

int i,j,k;
uint8_t more = 0;

int main(void)
{
	uart_init(); // from serialCommunication, starts UART and redirects STDOUT/IN
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	
	avrPort * owChannel1;
	avrPort Channel1;
	owChannel1 = &Channel1;
	owChannel1->port=&PORTC;
	owChannel1->pin=&PINC;
	owChannel1->ddr=&DDRC;
	owChannel1->mask=5;	

	avrPort * owChannel2;
	avrPort Channel2;
	owChannel2 = &Channel2;
	owChannel2->port=&PORTC;
	owChannel2->pin=&PINC;
	owChannel2->ddr=&DDRC;
	owChannel2->mask=4;	
	
	avrPort * owChannel3;
	avrPort Channel3;
	owChannel3 = &Channel3;
	owChannel3->port=&PORTC;
	owChannel3->pin=&PINC;
	owChannel3->ddr=&DDRC;
	owChannel3->mask=3;	
	
	avrPort * owChannel[3] = {owChannel1, owChannel2, owChannel3};

	// TRISTATE THIS PORT.
	//cbi(*owChannel[2]->ddr, owChannel[2]->mask);
	//cbi(*owChannel[2]->port, owChannel[2]->mask);
	
	int rc;
	int key_press;
	int conv_start = 0;
	int debug = 1;
	int active = 0;
	uint8_t my_addr[2][8];
	int prog_cntr = 0;
	uint8_t sp[10];
	uint8_t * scope;
	uint8_t first = 1;
	
	uint8_t saddr[64];
	
	printf("The program was restarted ...\n\n");
	
	while(1)
	{
		
		if (first){key_press = 'h'; first = 0;}
		else {key_press = wait_for_key(1);}
			
		prog_cntr++;
		switch(key_press) {
			case 'h': 
					printf("\n");
					// printf("s: SKIP_ROM (0x%02x)\n",SKIP_ROM);
					printf_P(PSTR("/: submitting the last read address with a MATCH_ROM\n"));
					printf_P(PSTR("?: print the active channel\n"));
					printf_P(PSTR(".: submitting READ_SCATCHPAD( 0x%02x)\n"),READ_SCRATCHPAD);
					printf_P(PSTR("0: write 0 to \n"));
					printf_P(PSTR("1: write 1 to \n"));
					printf_P(PSTR("b: read 1 bit \n"));
					printf_P(PSTR("a: READ_ROM (0x%02x)\n"),READ_ROM);
					printf_P(PSTR("c: CONVERT_T (0x%02x) and wait 1ms\n"),CONVERT_T);
					printf_P(PSTR("d: toggle debug, currently debug is:%u\n"),debug);
					printf_P(PSTR("e: temperature conversion without reading the status (good for parasite power)\n"));
					printf_P(PSTR("f: SEARCH_RIM (includes an init)\n"));
					printf_P(PSTR("h: this help\n"));
					printf_P(PSTR("i: initialize (reset)\n"));
					printf_P(PSTR("p: submit READ_POWER_SUPPLY (0x%02x)\n"),READ_POWER_SUPPLY);
					printf_P(PSTR("t: READ_SCRATCHPAD (0x%02x) , READ 2 bytes, print temperature\n"),READ_SCRATCHPAD);
				break;
			case '0': 
					transmit_bit(0,owChannel[active]);
					printf("0");
				break;
			case '1':
					transmit_bit(1,owChannel[active]);
					printf("1");
				break;
			case ' ':
				printf(" ");
				break;
			case '.':
				printf("%u: send READ_SCRATCHPAD (0x%02x) to channels %u: 0x%02x\n",READ_SCRATCHPAD, active,prog_cntr,READ_SCRATCHPAD);
				transmit_data(READ_SCRATCHPAD,owChannel[active]);
				break;
			case '/':
				printf("MATCH_ROM (0x%02x), selecting  channel 1: %0x.%02x%02x%02x%02x%02x%02x%02x\n"
					,MATCH_ROM
					,my_addr[active][0]
					,my_addr[active][1]
					,my_addr[active][2]
					,my_addr[active][3]
					,my_addr[active][4]
					,my_addr[active][5]
					,my_addr[active][6]
					,my_addr[active][7]
					);
					transmit_data(MATCH_ROM,owChannel1); 
					transmit_data(my_addr[active][0],owChannel[active]); 
					transmit_data(my_addr[active][1],owChannel[active]); 
					transmit_data(my_addr[active][2],owChannel[active]); 
					transmit_data(my_addr[active][3],owChannel[active]); 
					transmit_data(my_addr[active][4],owChannel[active]); 
					transmit_data(my_addr[active][5],owChannel[active]); 
					transmit_data(my_addr[active][6],owChannel[active]); 
					transmit_data(my_addr[active][7],owChannel[active]); 
				break;
			case '?':
				printf("active channel is %u\n",active+1);
				break;
			case 'a': 
				printf("%u: READ_ROM channel %u: 0x%02x\n",prog_cntr,active,READ_ROM);
				printf("reading address: ");
				transmit_data(READ_ROM,owChannel[active]); 
				for (i=0; i<8; i++){
					my_addr[active][i] = read_data(owChannel[active]);
				}
				for (i=0; i<8; i++){
					printf("%02x",my_addr[active][i]);
				}
				printf("\n");
				break;
			case (char) 13:
				printf("\n");
				break;
			case 'B':
				printf(" 0x%02x",read_data(owChannel[active]));
				break;
			case 'b':
				printf("%x",read_bit(owChannel[active]));
				break;
			case 'c':
				printf("%u: CONVERT_T: 0x%02x\n",prog_cntr,CONVERT_T);
				conv_start = 0;
				transmit_data(CONVERT_T,owChannel[active]); // Tell the sensor to prepare the data, 0x44 = 68
				
				// drive  low z '1' for parasite power devices
				sbi(*owChannel[2]->ddr, owChannel[2]->mask);
				sbi(*owChannel[2]->port, owChannel[2]->mask);
				
				// how soon after receiving CONVERT_T does it actually start?
				_delay_us(1000); 
				
				if (!read_bit(owChannel[active])){conv_start = 1;}
				while (!read_bit(owChannel[active])){
					conv_start++;
					_delay_us(10); 
				}
				
				cbi(*owChannel[2]->ddr, owChannel[2]->mask);
				cbi(*owChannel[2]->port, owChannel[2]->mask);
				
				if (conv_start){ printf("The temperature conversion completed after %u loops.\n",conv_start);}
				else {printf("The temperature conversion never started!\n");}
				
				break;
			case 'd':
				debug = !debug;
				printf("debug = %u\n",debug);
				break;
			case 'e':
				printf("%u: CONVERT_T (0x%02x) on channel %u without reading status (good for parasite power)\n",prog_cntr,CONVERT_T,active+1);
				conv_start = 0;
				transmit_data(CONVERT_T,owChannel[active]); // Tell the sensor to prepare the data, 0x44 = 68
				printf("the conversion was started.\n");
				_delay_ms(1000);
				printf("the conversion SHOULD be done now.\n");
				break;
			case 'f':
				printf("%u: SEARCH_ROM (0x%02x) on channel %u :  \n",prog_cntr,SEARCH_ROM,active+1);
				for (i=0;i<64;i++){
					saddr[i] = 4;
				}
				do {
					sensor_init(owChannel[active]);
					more = search_rom(saddr, owChannel[active]);
					switch (more){
						case 0: case 2:
							for (i=0; i<8; i++){
								for (j=0;j<4;j++){
									printf("%u",saddr[i*8+4+3-j]>0);
								}
								for (j=0;j<4;j++){
									printf("%u",saddr[i*8+3-j]>0);
								}
								printf(" ");
							}
							printf("\n");
							break;
						case 8:
							printf_P(PSTR("there are no devices on this channel.\n"));
							break;
					}
						
				} while (more & 0x6);
				printf_P(PSTR("all addresses found\n"));
				/*
				for (i=0; i<64;i++){
					if (!(i % 4)){printf(" ");}
					printf("%u",saddr[i]);
				}
				printf("\n");
				*/
				/*
				for (i=0; i<8; i++){
					k=0;
					for (j=0;j<4;j++){
						k |= saddr[i*8+j+4] << j;
					}
					printf("%x",k);
					k=0;
					for (j=0;j<4;j++){
						k |= saddr[i*8+j] << j;
					}
					printf("%x ",k);
				}
				printf("\n");
				*/
				break;
			case 'i':
				printf("%u: reset/presence detect on channel %u \n",prog_cntr,active+1);
				rc= sensor_init(owChannel[active]);
				if(rc)
				{
					printf("     -> sensor init failed with return code %u\n",rc);
				}
				else
				{
					if (debug) {
						printf("    -> sensor init succeeded\n");
					}
				}
				break;
			case 'j':
				scope = debug_sensor_init(owChannel[active]);
				for (i=0; i<480; i++){
					
					if (!(i % 100)){printf("\n%03u : ",i);}
					
					if (scope[i]){printf("1");}
					else {printf("0");}
				}
				printf("\n");
				break;
			case 'n':
				printf("\n");
				break;
			case 'p':
				printf("submit READ_POWER_SUPPLY (0x%02x) to channel %u\n",READ_POWER_SUPPLY,active);
				break;
			case 's':
				printf("%u: SKIP_ROM: 0x%02x\n",prog_cntr, SKIP_ROM);
				transmit_data(SKIP_ROM,owChannel[active]);
				break;
			case 't':
				printf("%u: READ_SCRATCHPAD: 0x%02x\n",prog_cntr,READ_SCRATCHPAD);
				transmit_data(READ_SCRATCHPAD,owChannel[active]);
				printf("reading scratchpad: ");
				for (i=0; i<9; i++){
					sp[i] = read_data(owChannel[active]);
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
			
			case 'x':
				active = (active+1) % 2;
				printf("active channel is now %u\n",active+1);
				break;
		}
	 }		
}

uint8_t search_rom(uint8_t saddr[64], avrPort * owChannel)
{
	/*
	0: only device(s) with a 0 in this position
	1: only device(s) with a 1 in this position
	2: at least one device with a 1 and one device with a 0
	3: no device on the channel
	*/ 
	uint8_t i,rc_t,rc_c;
	uint8_t more_to_do = 0;
	transmit_data(SEARCH_ROM,owChannel);
	for (i=0; i<64;i++){
		rc_t = read_bit(owChannel);
		rc_c = read_bit(owChannel);
		if (saddr[i] == 4){ // testing a new portion of the address
			if      (!rc_t && !rc_c){
				saddr[i] = 2; 
				transmit_bit(0,owChannel);
				//printf_P(PSTR("%u:00->2, submitting 0\n"),i);
			}
			else if (!rc_t &&  rc_c){
				saddr[i] = 0;
				transmit_bit(0,owChannel);
				//printf_P(PSTR("%u:01->0, submitting 0\n"),i);
			}
			else if ( rc_t && !rc_c){
				saddr[i] = 1;
				transmit_bit(1,owChannel);
				//printf_P(PSTR("%u:10->1, submitting 1\n"),i);
			}
			else if ( rc_t &&  rc_c){
				saddr[i] = 8;
				printf_P(PSTR("%u:abort\n"),i);
				return(8);
			}
		} else {
			if (saddr[i] & 16){ // last decision point found before and taken the '0' path
				saddr[i] = 1; 
				//printf_P(PSTR("I found a 2 in position %u, now taking the 1\n"),i);
				//printf_P(PSTR("rc+t=%u, rc_c = %u\n"),rc_t,rc_c);
				transmit_bit(1,owChannel);
				for (rc_t=i+1;rc_t<64;rc_t++){
					saddr[rc_t] = 4;
				}
			} else  {
				// transmit a '1', everything else is 0
				transmit_bit(saddr[i] & 1 ,owChannel);
			}
		}
	}
	more_to_do = 0;
	for (i=0;i<64;i++){
		more_to_do |= saddr[i] & 0x6; // either 2 or 4
	}
	for (i=64;i>0;i--){
		if (saddr[i-1] & 2){
			saddr[i-1] |= 16; // mark last decision in the string
			break;
		}
			
	}
	return(more_to_do);
}
