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
clockTime sys_time={.hour=8,.minute=59,.second=55,.day=5};


bool newSecond = false;

uint8_t statusRF;
char Address[_Address_Width] = { 0x11, 0x22, 0x33, 0x44, 0x55};
char spi_rx_buf[_Buffer_Size] ;
char spi_tx_buf[_Buffer_Size] ;	
int main (void)
{
	board_init();

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
				sys_time.day ++;
				if (sys_time.day == 7)
				{
					sys_time.day = 0;
				}
			}
		}
	}
	ioport_toggle_pin_level(LED_GREEN);
	newSecond = true;
	updateTemperature();
	valve_manager(sys_time);
	wdt_reset(); 
}

void usb_connection(void)
{
	int temp = getTemperature()*10;
	if (udd_is_underflow_event())
	{
		char usb_out [100];
		uint8_t count = sprintf(usb_out, "Time : %d:%d:%d || temp : %d \r",sys_time.hour,sys_time.minute,sys_time.second, temp);
		for (int i=0;i<count;i++)
		{
			udi_cdc_putc(usb_out[i]);
		}

		count = sprintf(usb_out, "     HOT1      => Frequency: %d || Time : %d:%d:%d \r", wsHot[0].frequency, wsHot[0].openTime.hour, wsHot[0].openTime.minute, wsHot[0].openTime.second);
		for (int i=0;i<count;i++)
		{
			udi_cdc_putc(usb_out[i]);
		}
		
		count = sprintf(usb_out, "     HOT2      => Frequency: %d || Time : %d:%d:%d \r", wsHot[1].frequency, wsHot[1].openTime.hour, wsHot[1].openTime.minute, wsHot[1].openTime.second);
		for (int i=0;i<count;i++)
		{
			udi_cdc_putc(usb_out[i]);
		}
		
		count = sprintf(usb_out, "     Temperate => Frequency: %d || Time : %d:%d:%d \r", wsTemperate[0].frequency, wsTemperate[0].openTime.hour, wsTemperate[0].openTime.minute, wsTemperate[0].openTime.second);
		for (int i=0;i<count;i++)
		{
			udi_cdc_putc(usb_out[i]);
		}
		
		count = sprintf(usb_out, "     Cold      => Frequency: %d || Time : %d:%d:%d \r", wsCold[0].frequency, wsCold[0].openTime.hour, wsCold[0].openTime.minute, wsCold[0].openTime.second);
		for (int i=0;i<count;i++)
		{
			udi_cdc_putc(usb_out[i]);
		}
	}
}