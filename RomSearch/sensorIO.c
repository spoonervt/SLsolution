/*
 * CFile1.c
 *
 * Created: 12/10/2015 9:34:09 PM
 *  Author: steffen
 */ 

#include <avr/io.h>
#include "sensorIO.h"
#include "avrPort.h"



void write_sensor( uint8_t data,  avrPort * owChannel)
{
	// pin D6 is for debugging ..->
	if (data){ // write a 1: configure as input with pullup active
		cbi(*owChannel->ddr, owChannel->mask);    // input
		sbi(*owChannel->port, owChannel->mask); // enable pull-up
		
		//  this is used to show the logic without the  wired and
		cbi(*owChannel->ddr,6);
		sbi (*owChannel->port, 6);
		//sbi(*owChannel->ddr, owChannel->mask);`
		//sbi(*owChannel->port,owChannel->mask);
	}
	else	{ //write 0: configure as output driving low
		sbi(*owChannel->ddr, owChannel->mask); // output
		cbi(*owChannel->port,owChannel->mask); // drive 0

		//  this is used to show the logic without the  wired and
		sbi(*owChannel->ddr, 6);
		cbi(*owChannel->port,6);
	}
}

int read_sensor(  avrPort * owChannel)
{
		cbi(*owChannel->ddr,owChannel->mask);    // input
		sbi (*owChannel->port, owChannel->mask); // enable pull-up
		return *owChannel->pin & (1 << owChannel->mask);
}
