/*
 * owCommands.c
 *
 * Created: 12/20/2015 9:14:44 PM
 *  Author: steffen
 */ 



#include "avrPort.h"
#include "sensorIO.h"
#include "owCommands.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>



uint8_t sensor_init(avrPort * owChannel)
{

	int i;
	uint8_t presence_detected = 0;
	write_sensor(0,owChannel);
	_delay_us(RESET_PULSE_US);
	write_sensor(1,owChannel);

	// slave waits  SLAVE_WAITS_RISING_EDGE_MIN_US for rising edge 
	// and may start driving low then
	_delay_us(SLAVE_WAITS_RISING_EDGE_MIN_US);  

	//wait until slave actually pulls low 
	for (i=0; i<300;i++){
		_delay_us(1);
		if (!read_sensor(owChannel)){
			presence_detected = 1;
		}
	}
	
	_delay_us(RESET_PULSE_US - SLAVE_WAITS_RISING_EDGE_MAX_US - PRESENCE_PULSE_DURATION_MAX_US);
	
	//PORTD &= ~1<<2;
	
	if (presence_detected){return SENSOR_INIT_VALIDATION_SUCCESS;}
	else {return SENSOR_INIT_VALIDATION_FAIL_1;}
}
uint8_t * debug_sensor_init(avrPort * owChannel)
{
	static uint8_t scope[480];
	uint8_t * p;
	p = scope;

	write_sensor(0,owChannel);
	_delay_us(RESET_PULSE_US);
	write_sensor(1,owChannel);

	int i;
	for ( i=0; i<RESET_PULSE_US; i++){
		scope[i] = read_sensor(owChannel);
	}
	return p;
}

void transmit_data(uint8_t data,  avrPort * owChannel)
{
	uint8_t bitmask = 0x01;
	
	for (uint8_t current_bit = 0; current_bit < 8; current_bit++)
	{
		transmit_bit(data & (bitmask << current_bit),owChannel);
	}
}
uint8_t read_data( avrPort * owChannel)
{
	uint8_t data = 0x00;
	uint8_t mask = 0x01;
	for (uint8_t num_bits = 0; num_bits < 8; num_bits++)
	{
		if(read_bit(owChannel))
		{
			data |= mask<<num_bits;
		}
	}
	return data;
}
void sensor_port_init(avrPort * owChannel)
{
	write_sensor(1,owChannel);
	_delay_ms(1);
}

void transmit_bit(uint8_t bit, avrPort * owChannel){

	if (bit)
	{
		// write 1
		write_sensor(0,owChannel);
		_delay_us(OW_TLOW1);
		write_sensor(1,owChannel);
		_delay_us(OW_TSLOT-OW_TLOW1);
	}
	else // write 0
	{
		write_sensor(0,owChannel);
		_delay_us(OW_TLOW0);
	}
	write_sensor(1,owChannel);
	_delay_us(OW_TREC);
}
uint8_t read_bit(avrPort * owChannel){
	uint8_t bit;
	

	write_sensor(0,owChannel); // pull bus down. initiate transfer
	_delay_us(OW_TREAD);
	write_sensor(1,owChannel); // release bus,
	_delay_us(OW_TSAMPLE_WAIT);			// now read status
	
	// this is the read ...
	bit = *owChannel->pin & (1<< owChannel->mask);

	if(bit)
	{
		bit = 1;
	}
	else
	{
		bit = 0;
	}
	_delay_us(OW_TSLOT - OW_TREAD - OW_TSAMPLE_WAIT);
	_delay_us(OW_TREC);
	return (bit);
}

void write_sensor( uint8_t data,  avrPort * owChannel)
{
	// pin D6 is for debugging ..->
	if (data){ // write a 1: configure as input with pullup active
		cbi(*owChannel->ddr, owChannel->mask);    // input
		sbi(*owChannel->port, owChannel->mask); // enable pull-up
		
		//  this is used to show the logic without the  wired and
		// cbi(*owChannel->ddr,6);
		// sbi (*owChannel->port, 6);
	}
	else	{ //write 0: configure as output driving low
		sbi(*owChannel->ddr, owChannel->mask); // output
		cbi(*owChannel->port,owChannel->mask); // drive 0

		//  this is used to show the logic without the  wired and
		// sbi(*owChannel->ddr, 6);
		//  cbi(*owChannel->port,6);
	}
}

int read_sensor(  avrPort * owChannel)
{
	cbi(*owChannel->ddr,owChannel->mask);    // input
	sbi (*owChannel->port, owChannel->mask); // enable pull-up
	return *owChannel->pin & (1 << owChannel->mask);
}

