/*
 * owCommands.h
 *
 * Created: 12/28/2015 1:16:04 AM
 *  Author: steffen
 */ 


#ifndef OWCOMMANDS_H_
#define OWCOMMANDS_H_

#include "avrPort.h"

// Reset pulse duration
#define RESET_PULSE_US					480

#define PRESENCE_PULSE_DURATION_MIN_US	60
#define PRESENCE_PULSE_DURATION_MAX_US	240

#define SLAVE_WAITS_RISING_EDGE_MIN_US	15
#define SLAVE_WAITS_RISING_EDGE_MAX_US	60

#define SENSOR_INIT_VALIDATION_SUCCESS	0
#define SENSOR_INIT_VALIDATION_FAIL_1	1
#define SENSOR_INIT_VALIDATION_FAIL_2	2
#define SENSOR_INIT_VALIDATION_FAIL_3	3

#define OW_TSLOT						60
#define OW_TLOW0						60
#define OW_TLOW1                         3
#define OW_TREC                         10
#define OW_TREAD						 6
#define OW_TSAMPLE_WAIT					 4



#define CONVERT_T						0x44
#define SKIP_ROM						0xCC
#define READ_SCRATCHPAD					0xBE
#define READ_ROM						0x33
#define MATCH_ROM						0x55
#define SEARCH_ROM						0xF0
#define READ_POWER_SUPPLY				0xB4


extern uint8_t sensor_init(avrPort * owChannel);
extern void transmit_bit(uint8_t bit, avrPort * owChannel);
extern void transmit_data(uint8_t data,  avrPort * owChannel);
extern uint8_t read_data( avrPort * owChannel);
extern void sensor_port_init(avrPort * owChannel);
extern void write_sensor( uint8_t data,  avrPort * owChannel);
extern int read_sensor(  avrPort * owChannel);
extern uint8_t read_bit(avrPort  * owChannel);
extern uint8_t * debug_sensor_init(avrPort * owChannel);


#endif /* OWCOMMANDS_H_ */