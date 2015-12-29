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


void write_sensor( uint8_t data,  avrPort * owChannel);
int read_sensor(  avrPort * owChannel);


#endif /* SENSORIO_H_ */