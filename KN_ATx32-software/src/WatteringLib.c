/*
 * WatteringLib.c
 *
 * Created: 23/03/1396 11:33:11 ب.ظ
 *  Author: Eagle
 */ 

#include "WatteringLib.h"
float temperature = 10;
float lastMaxTemperature = 4;
uint32_t secondsBetweenWatterings = 84600;
uint32_t openTimer = 50400;//seconds till next wattering time
uint16_t closeTimer = 0;
watterSchedule * nextCloseSchedule;

watterSchedule wsHot[HOT_WATERING_TIMES] = {
	{
		.dayOfWeek={1,1,1,1,1,1,1},
		.openTime.hour = 9,
		.openTime.minute = 0,
		.openTime.second = 0,
		.closeTime.hour   = 9,
		.closeTime.minute = 0 ,
		.closeTime.second = 35,
		.frequency = 0,
		.valveNumber = 2
	},
	{
		.dayOfWeek={1,1,1,1,1,1,1},
		.openTime.hour = 21,
		.openTime.minute = 0,
		.openTime.second = 0,
		.closeTime.hour   = 21,
		.closeTime.minute = 0 ,
		.closeTime.second = 35,
		.frequency = 0,
		.valveNumber = 2
	}
};

watterSchedule wsTemperate[TEMPERATE_WATERING_TIMES] = {
	{
		.dayOfWeek={1,1,1,1,1,1,1},
		.openTime.hour = 9,
		.openTime.minute = 0,
		.openTime.second = 0,
		.closeTime.hour   = 9,
		.closeTime.minute = 0 ,
		.closeTime.second = 25,
		.frequency = 0,
		.valveNumber = 2
	}
};

watterSchedule wsCold[COLD_WATERING_TIMES] = {
	{
		.dayOfWeek={1,0,0,1,0,0,0},
		.openTime.hour = 9,
		.openTime.minute = 0,
		.openTime.second = 0,
		.closeTime.hour   = 9,
		.closeTime.minute = 0 ,
		.closeTime.second = 25,
		.frequency = 0,
		.valveNumber = 2
	}
};


void watterScheduleCheck(clockTime sys_time, watterSchedule * ws){
	if (timeEqualityCheck(sys_time, ws->openTime, true, ws->dayOfWeek)){
		e_valve(ws->valveNumber,OPEN);
		nextCloseSchedule = ws;
	}
}

void e_valve (uint8_t valve_number, bool state)
{
	if (state == OPEN)
	{
		switch (valve_number)
		{
			case 1:
			ioport_set_pin_high(DRIVER_ENA);
			ioport_set_pin_high(DRIVER_IN1);
			ioport_set_pin_low(DRIVER_IN2);
			ioport_set_pin_low(LED_BLUE);
			break;
			
			case 2:
			ioport_set_pin_high(DRIVER_ENB);
			ioport_set_pin_high(DRIVER_IN3);
			ioport_set_pin_low(DRIVER_IN4);
			ioport_set_pin_low(LED_BLUE);
			break;
			
			default:
			break;
		}
	}
	
	if (state == CLOSE)
	{
		switch (valve_number)
		{
			case 1:
			ioport_set_pin_low(DRIVER_ENA);
			ioport_set_pin_high(DRIVER_IN1);
			ioport_set_pin_low(DRIVER_IN2);
			ioport_set_pin_high(LED_BLUE);
			break;
			
			case 2:
			ioport_set_pin_low(DRIVER_ENB);
			ioport_set_pin_high(DRIVER_IN3);
			ioport_set_pin_low(DRIVER_IN4);
			ioport_set_pin_high(LED_BLUE);
			break;
			
			default:
			break;
		}
	}
	
}

void valve_manager (clockTime sys_time)
{
	int temp = getTemperature();
	if (temp > lastMaxTemperature){
		lastMaxTemperature = temp;
	}

	if (openTimer>=1){
		openTimer--;
	}

	if (closeTimer>=1){
		closeTimer--;
	}

	if ((60 > temp ) && (temp >= 4) && (openTimer==0)){
		e_valve(2,OPEN);
		closeTimer = 30;
		openTimer = secondsBetweenWatterings;
	}
	
	if (closeTimer==0){
		e_valve(2,CLOSE);
	}

}

void manualWattering(void){
	if (!ioport_get_pin_level(BUTTON_0))
	{
		e_valve(2,OPEN);
		delay_ms(3);
		while(!ioport_get_pin_level(BUTTON_0));
		e_valve(2,CLOSE);
	}
}

bool timeEqualityCheck(clockTime sys_time, clockTime wsTime, bool secondCheck,  bool * dayOfWeek ){
	
	if (!dayOfWeek[sys_time.day])
	{
		return false;
	}
	
	if (sys_time.hour != wsTime.hour)
	{
		return false;
	}
	if (sys_time.minute != wsTime.minute)
	{
		return false;
	}
	if (sys_time.second != wsTime.second && secondCheck)
	{
		return false;
	}
	return true;
}


void updateTemperature(void){
	adc_start_conversion(&LM35_ADC, LM35_ADC_CH);
	adc_wait_for_interrupt_flag(&LM35_ADC, LM35_ADC_CH);
	temperature = (adc_get_result(&LM35_ADC, LM35_ADC_CH)*100/2048 - temperature)*0.1 + temperature;
}

float getTemperature(void){
	return temperature;
}

void resetMaxTemp(void){

	secondsBetweenWatterings = (uint32_t)(3203616.1*pow(lastMaxTemperature, -1.3724));  
	lastMaxTemperature = 4;
}
	

