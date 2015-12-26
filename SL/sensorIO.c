/*
 * CFile1.c
 *
 * Created: 12/10/2015 9:34:09 PM
 *  Author: steffen
 */ 

#include <avr/io.h>
#include "sensorIO.h"

struct avr_port;

void write_sensor( uint8_t data, avr_port sensor)
{
	// pin D6 is for debugging ...
	if (data){ // write a 1: configure as input with pullup active
		cbi(*sensor.ddr,sensor.mask);    // input
		sbi (*sensor.port, sensor.mask); // enable pull-up
		
		//  this is used to show the logic without the  wired and
		cbi(*sensor.ddr,6);
		sbi (*sensor.port, 6);
		//sbi(*sensor.ddr, sensor.mask);
		//sbi(*sensor.port,sensor.mask);
	}
	else	{ //write 0: configure as output driving low
		sbi(*sensor.ddr, sensor.mask); // output
		cbi(*sensor.port,sensor.mask); // drive 0

		//  this is used to show the logic without the  wired and
		sbi(*sensor.ddr, 6);
		cbi(*sensor.port,6);
	}
}

int read_sensor(avr_port sensor)
{
		cbi(*sensor.ddr,sensor.mask);    // input
		sbi (*sensor.port, sensor.mask); // enable pull-up
		return *sensor.pin & (1 << sensor.mask);
}
