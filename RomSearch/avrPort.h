/*
 * avrPort.c
 *
 * Created: 12/20/2015 9:19:09 PM
 *  Author: steffen
 */ 

#include <avr/io.h>

typedef struct {
	volatile uint8_t *port;
	volatile uint8_t *pin;
	volatile uint8_t *ddr;
	uint8_t mask;
} avr_port;

