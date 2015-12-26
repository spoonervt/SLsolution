/*
 * ds18S20_driver.c
 *
 * Created: 14.12.2013 03:06:56
 *  Author: Irfan Sehic
 */ 

#include "ds18S20.h"
#include "sensorIO.h"

void sensor_port_init(avr_port sensor) 
{
	write_sensor(1,sensor);
	_delay_ms(1);
}
uint8_t sensor_init(avr_port sensor)
{
	uint8_t ack = 0;

	write_sensor(0,sensor);
	_delay_us(600);

	write_sensor(1,sensor);
	_delay_us(100);
	DDRD |= 1<<2;
	PORTD |= 1<<2;
	ack = read_sensor(sensor);
	PORTD &= ~1<<2;
	_delay_us(1);
	_delay_us(499);
	
	if(ack == 0)
		return SENSOR_INIT_VALIDATION_SUCCESS;
	else
		return SENSOR_INIT_VALIDATION_FAIL;
}
void transmit_bit(uint8_t bit, avr_port sensor){
	_delay_us(100);
	if (bit) 
		{
			// write 1
			write_sensor(0,sensor);
			_delay_us(10);
			write_sensor(1,sensor);
			_delay_us(50);
		}
		else // write 0
		{
			write_sensor(0,sensor);
			_delay_us(60);
		}
	write_sensor(1,sensor);
}
void transmit_data(uint8_t data, avr_port sensor)
{
	uint8_t bitmask = 0x01;
	
	for (uint8_t current_bit = 0; current_bit < 8; current_bit++)
	{
  		transmit_bit(data & (bitmask << current_bit),sensor);
	}
	_delay_us(500);
}
uint8_t read_data(avr_port sensor)
{
	uint8_t data = 0x00;
	uint8_t mask = 0x01;
	uint8_t bit;
	for (uint8_t num_bits = 0; num_bits < 8; num_bits++)
	{
		_delay_us(1);
		write_sensor(0,sensor); // pull bus down
		_delay_us(2);
		write_sensor(1,sensor); // release bus
		_delay_us(1); // this used to be 5 but this is way too late ...
		
		// 2: orange (channel 2: latch timing)
		// 3: yellow (channel 3): sensed data 
		DDRD |= 1<<2 | 1<<3;
		PORTD |= 1<<2 ; //rising edge shows where I latch data
		bit = *sensor.pin & (1<<sensor.mask);
		if(bit)
		{
			PORTD |= 1<<3 ;
			data |= mask<<num_bits;
		}
		else
		{
			// do nothing, init was with 0
		} /**/
		_delay_us(3);
		
		// turn all debugs off
		PORTD &= ~( 1<<2 | 1 << 3);
		
		_delay_us(45);
	}
	return data;
}
