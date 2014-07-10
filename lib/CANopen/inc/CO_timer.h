/*
 * timer.h
 *
 *  Created on: 13.9.2012
 *      Author: one
 */

#ifndef TIMER_H_
#define TIMER_H_
#include "CO_driver.h"

typedef unsigned int (*pf_getTimerVal_us)();

typedef struct {
	uint32_t savedTime;
} ttimer;

int initTimer(pf_getTimerVal_us gett);
void saveTime(ttimer *tim);
uint32_t getTime_ms(ttimer *tim);
uint32_t getTime_us(ttimer *tim);

#endif /* TIMER_H_ */
