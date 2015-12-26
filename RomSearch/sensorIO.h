/*
 * sensorIO.h
 *
 * Created: 12/10/2015 9:36:35 PM
 *  Author: steffen
 */ 


#ifndef SENSORIO_H_
#define SENSORIO_H_

#include "avrPort.h"


#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

typedef struct {
	volatile uint8_t *port;
	volatile uint8_t *pin;
	volatile uint8_t *ddr;
	uint8_t mask;
} avr_port;


void write_sensor( uint8_t data, avr_port sensor);
int read_sensor( avr_port sensor);


#endif /* SENSORIO_H_ */