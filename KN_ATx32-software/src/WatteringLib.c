/*
 * WatteringLib.c
 *
 * Created: 23/03/1396 11:33:11 ب.ظ
 *  Author: Eagle
 */ 

#include "WatteringLib.h"

watterSchedule watterScheduleCheck(clockTime sys_time, watterSchedule ws){
	if (timeEqualityCheck(sys_time, ws.openTime, true)){
		e_valve(ws.valveNumber,OPEN);
	}
	if(timeEqualityCheck(sys_time, ws.closeTime, true)){
		e_valve(ws.valveNumber,CLOSE);
	}
	if(timeEqualityCheck(sys_time, ws.closeTime, true)){
		ws.frequency++;
	}
	return ws;
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

void valve_manager (clockTime sys_time, watterSchedule * ws)
{
	for (int i = 0; i < WATERING_TIMES; i++)
	{
		ws[i] = watterScheduleCheck(sys_time, ws[i]);
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

bool timeEqualityCheck(clockTime time1, clockTime time2, bool secondCheck){
	if (time1.hour != time2.hour)
	{
		return false;
	}
	if (time1.minute != time2.minute)
	{
		return false;
	}
	if (time1.second != time2.second && secondCheck)
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
	

