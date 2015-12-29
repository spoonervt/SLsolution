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


uint8_t sensor_init(avrPort * owChannel)
{
	uint8_t ack = 0;

	write_sensor(0,owChannel);
	_delay_us(600);

	write_sensor(1,owChannel);
	_delay_us(100);
	DDRD |= 1<<2;
	PORTD |= 1<<2;
	ack = read_sensor(owChannel);
	PORTD &= ~1<<2;
	_delay_us(1);
	_delay_us(499);
	
	if(ack == 0)
	return SENSOR_INIT_VALIDATION_SUCCESS;
	else
	return SENSOR_INIT_VALIDATION_FAIL;
}
void transmit_bit(uint8_t bit, avrPort * owChannel){
	_delay_us(100);
	if (bit)
	{
		// write 1
		write_sensor(0,owChannel);
		_delay_us(10);
		write_sensor(1,owChannel);
		_delay_us(50);
	}
	else // write 0
	{
		write_sensor(0,owChannel);
		_delay_us(60);
	}
	write_sensor(1,owChannel);
	_delay_us(1);
}
void transmit_data(uint8_t data,  avrPort * owChannel)
{
	uint8_t bitmask = 0x01;
	
	for (uint8_t current_bit = 0; current_bit < 8; current_bit++)
	{
		transmit_bit(data & (bitmask << current_bit),owChannel);
	}
	_delay_us(500);
}
uint8_t read_data( avrPort * owChannel)
{
	uint8_t data = 0x00;
	uint8_t mask = 0x01;
	for (uint8_t num_bits = 0; num_bits < 8; num_bits++)
	{
/*		_delay_us(1);
		write_sensor(0,owChannel); // pull bus down
		_delay_us(2);
		write_sensor(1,owChannel); // release bus
		_delay_us(5);
		
		// 2: orange (channel 2: latch timing)
		// 3: yellow (channel 3): sensed data
		DDRD |= 1<<2 | 1<<3;
		PORTD |= 1<<2 ; //rising edge shows where I latch data
		if(read_sensor(owChannel))
		{
			data |= mask<<num_bits;
			PORTD |= 1<<3 ;
		}
		else
		{
			// do nothing, init was with 0
		}
		_delay_us(3);
		
		// turn all debugs off
		PORTD &= ~( 1<<2 | 1 << 3);
		
		_delay_us(45);
*/
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

uint8_t read_bit(avrPort * owChannel){
	uint8_t bit;
	
	_delay_us(1);
	write_sensor(0,owChannel); // pull bus down. initiate transfer
	_delay_us(2);
	write_sensor(1,owChannel); // release bus,
	_delay_us(1);			// now read status
	
	// 2: orange (channel 2: latch timing)
	// 3: yellow (channel 3): sensed data
	DDRD |= 1<<2 | 1<<3;
	bit = *owChannel->pin & (1<< owChannel->mask);
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

