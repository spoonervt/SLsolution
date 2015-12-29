/*
 * sbicbi.c
 *
 * Created: 12/20/2015 9:21:18 PM
 *  Author: steffen
 */ 


#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))
