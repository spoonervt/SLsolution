/*
 * ds18S20.h
 *
 * A mini library for the ds18S20 temperature sensor
 *
 * Created: 14.12.2013
 *  Author: Irfan Sehic
 */ 

#ifndef DS18S20_H_
#define DS18S20_H_

#include <avr/io.h>
#include <util/delay.h>
#include "sensorIO.h"

// Idle state of the bus is HIGH

// Port used for the communication with the sensor
#define SENSOR_PORT			PORTD
// DDR Register for definition of input (0) and output (1)
#define SENSOR_DDR			DDRD

// Port for writing to the bus
#define SENSOR_WRITE		PD7

// Port for reading from the bus
#define SENSOR_READ   		PD4

// Reset pulse duration
#define RESET_PULSE_US					480

#define PRESENCE_PULSE_DURATION_MIN_US	60
#define PRESENCE_PULSE_DURATION_MAX_US	240

#define SLAVE_WAITS_RISING_EDGE_MIN_US	15
#define SLAVE_WAITS_RISING_EDGE_MAX_US	60

#define SENSOR_INIT_VALIDATION_SUCCESS	0
#define SENSOR_INIT_VALIDATION_FAIL		1

#define CONVERT_T						0x44
#define SKIP_ROM						0xCC
#define READ_SCRATCHPAD					0xBE
#define READ_ROM						0x33
#define MATCH_ROM						0x55
#define SEARCH_ROM						0xF0

/**
   Initialize the sensor port 
   @return nothing
 */
extern void sensor_port_init(avr_port sensor);

/**
   Initialize the sensor
   @return validation status
 */
extern uint8_t sensor_init(avr_port sensor);

/**
   Transmit 1 bit of data to the sensor
   @param[data] data to be sent
   @return nothing
 */
extern void transmit_bit(uint8_t bit,avr_port sensor);

/**
   Transmit data to the sensor
   @param[data] data to be sent
   @return nothing
   calls transmit 8 times
 */
extern void transmit_data(uint8_t data,avr_port sensor);

/**
   Read data from the sensor
   @return data which was read
 */
extern uint8_t read_data(avr_port sensor);

#endif /* DS18S20_H_ */