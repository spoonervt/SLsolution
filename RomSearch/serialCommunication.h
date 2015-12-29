/*
 * serialCommunication.h
 *
 * Created: 12/28/2015 1:28:34 AM
 *  Author: steffen
 */ 


#ifndef SERIALCOMMUNICATION_H_
#define SERIALCOMMUNICATION_H_

uint8_t uart_getchar(void);
void uart_init();
uint8_t wait_for_key(uint8_t silent);


#endif /* SERIALCOMMUNICATION_H_ */