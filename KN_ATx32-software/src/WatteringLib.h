/*
 * WatteringLib.h
 *
 * Created: 23/03/1396 11:32:45 ب.ظ
 *  Author: Eagle
 */ 


#ifndef WATTERINGLIB_H_
#define WATTERINGLIB_H_

#include "asf.h"

#define OPEN  1
#define CLOSE 0
#define WATTERING_DURATION 30
#define WATERING_TIMES 2

typedef struct
{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}clockTime;

typedef struct {
	clockTime openTime;
	clockTime closeTime;
	uint8_t frequency;
	uint8_t valveNumber;
}watterSchedule;

typedef struct {
	uint8_t id;
	uint8_t commandReg;
	clockTime sytemTime;
	watterSchedule ws[3];
}wirelessPackage;

watterSchedule watterScheduleCheck(clockTime sys_time, watterSchedule ws);
void e_valve (uint8_t valve_number, bool state);
void valve_manager (clockTime sys_time, watterSchedule * ws);
void manualWattering(void);
bool timeEqualityCheck(clockTime time1, clockTime time2, bool secondCheck);


#endif /* WATTERINGLIB_H_ */