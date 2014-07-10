/*
 * timer.c
 *
 *  Created on: 13.9.2012
 *      Author: one
 */
 
#include "CO_timer.h"

static pf_getTimerVal_us gettimerfunc = 0;

int initTimer(pf_getTimerVal_us gett) {
	gettimerfunc = gett;
	return 0;
}

void saveTime(ttimer *tim) {
	if (!tim)
		return;
	tim->savedTime = gettimerfunc();
}

uint32_t getTime_ms(ttimer *tim) {
	if (!tim)
		return 0;
	return (gettimerfunc() - tim->savedTime) / 1000;
}

uint32_t getTime_us(ttimer *tim) {
	if (!tim)
		return 0;
	uint32_t now = gettimerfunc();
	return (now - tim->savedTime);
}
