/*
* Support and FAQ: visit <a href="http://www.kn2c.ir">KN2C Robotic Team</a>
*/
#include <asf.h>
#include <stdio.h>
#include <usb_device.h>
#include "WatteringLib.h"

void wireless_connection ( void );
void usb_connection(void);

enum RF_MODE {RX_MODE, TX_MODE}rfMode;
clockTime sys_time={.hour=6,.minute=52,.second=55};
watterSchedule ws[WATERING_TIMES];

bool newSecond = false;

uint8_t statusRF;
char Address[_Address_Width] = { 0x11, 0x22, 0x33, 0x44, 0x55};
char spi_rx_buf[_Buffer_Size] ;
char spi_tx_buf[_Buffer_Size] ;	
int main (void)
{
	board_init();
	ws[0].openTime.hour = 6;
	ws[0].openTime.minute = 0;
	ws[0].openTime.second = 0;
	ws[0].closeTime.hour   = ws[0].openTime.hour ;
	ws[0].closeTime.minute = ws[0].openTime.minute ;
	ws[0].closeTime.second = ws[0].openTime.second +WATTERING_DURATION;
	ws[0].frequency = 0;
	ws[0].valveNumber = 2;
	
	ws[1].openTime.hour = 18;
	ws[1].openTime.minute = 0;
	ws[1].openTime.second = 0;
	ws[1].closeTime.hour = ws[1].openTime.hour;
	ws[1].closeTime.minute = ws[1].openTime.minute;
	ws[1].closeTime.second = ws[1].openTime.second + WATTERING_DURATION;
	ws[1].frequency = 0;
	ws[1].valveNumber = 2;
	
	while(1)
	{
		delay_ms(20);
		manualWattering();
		if (newSecond)
		{
			usb_connection();
			newSecond = false;
		}
	}
}


ISR(PORTC_INT0_vect)//PRX   IRQ Interrupt Pin
{
	wireless_connection();
}

void wireless_connection ( void )
{

}




ISR(RTC_OVF_vect)
{
	sys_time.second ++;
	if(sys_time.second >= 60)
	{
		sys_time.second = 0;
		sys_time.minute ++;
		if (sys_time.minute == 60)
		{
			sys_time.minute = 0;
			sys_time.hour ++;
			if (sys_time.hour == 24)
			{
				sys_time.hour = 0;
			}
		}
	}
	ioport_toggle_pin_level(LED_GREEN);
	newSecond = true;
	valve_manager(sys_time,ws);
	wdt_reset(); 
}

void usb_connection(void)
{
	if (udd_is_underflow_event())
	{
		char usb_out [100];
		uint8_t count = sprintf(usb_out, "Time : %d:%d:%d \r",sys_time.hour,sys_time.minute,sys_time.second);
		for (int i=0;i<count;i++)
		{
			udi_cdc_putc(usb_out[i]);
		}
		for(int j = 0; j < WATERING_TIMES; j++){
			count = sprintf(usb_out, "     WatterSchedule%d => Frequency: %d || Time : %d:%d:%d \r", j, ws[j].frequency, ws[j].openTime.hour, ws[j].openTime.minute, ws[j].openTime.second);
			for (int i=0;i<count;i++)
			{
				udi_cdc_putc(usb_out[i]);
			}
		}
	}
}