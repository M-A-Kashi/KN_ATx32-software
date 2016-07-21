/*
 * Support and FAQ: visit <a href="http://www.kn2c.ir">KN2C Robotic Team</a>
 */
#include <asf.h>
#include <stdio.h>

#define  WATTERING_TIME_MINUTE 35
#define  WATTERING_TIME_HOUR 9
#define  WATTERING_DURATION 5
void wireless_connection ( void );
void clock_1s(void);
void e_valve(void);
struct clock_time
{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}sys_time={.hour=9,.minute=34,.second=55};

struct task
{
	bool wattering;
	bool lighting;
}today_task;
int main (void)
{
	board_init();

	while(1)
	{
		
		delay_ms(20);
		ioport_set_pin_level(LED_WHITE,HIGH);
		/*ioport_set_pin_level(LED_BLUE,HIGH);*/
		e_valve();
		
		if (!ioport_get_pin_level(BUTTON_0))
		{
			ioport_set_pin_high(DRIVER_ENB);
			ioport_set_pin_high(DRIVER_IN3);
			ioport_set_pin_low(DRIVER_IN4);
			ioport_set_pin_level(LED_BLUE,LOW);
			while(!ioport_get_pin_level(BUTTON_0))
			ioport_set_pin_low(DRIVER_ENB);
			ioport_set_pin_high(DRIVER_IN3);
			ioport_set_pin_low(DRIVER_IN4);
			ioport_set_pin_level(LED_BLUE,HIGH);
		}

		char usb_out [100];
		uint8_t count = sprintf(usb_out, "TIME : %d:%d:%d    %d \r",sys_time.hour,sys_time.minute,sys_time.second,RTC.CNT);
		for (int i=0;i<count;i++)
		{
			udi_cdc_putc(usb_out[i]);
		}
	}
}


ISR(PORTC_INT0_vect)//PRX   IRQ Interrupt Pin
{
	wireless_connection();
}

char spi_rx_buf[_Buffer_Size] ;
void wireless_connection ( void )
{
	uint8_t status = NRF24L01_WriteReg(W_REGISTER | STATUSe, _TX_DS|_MAX_RT|_RX_DR);
	if((status & _RX_DR) == _RX_DR)
	{
		ioport_set_pin_level(LED_WHITE,LOW);

		//wdt_reset();
		NRF24L01_Read_RX_Buf(spi_rx_buf, _Buffer_Size);
		if(spi_rx_buf[0] == MODULE_ID )
		{
			spi_rx_buf[0]=0;
			spi_rx_buf[1]=rtc_get_time();
			ioport_set_pin_level(LED_BLUE,LOW);
			NRF24L01_Write_TX_Buf(spi_rx_buf, _Buffer_Size);
			//signal_strength++;
		}
	}
	
	if ((status&_MAX_RT) == _MAX_RT)
	{
		NRF24L01_Flush_TX();
	}
}


void e_valve (void)
{
	if(sys_time.hour == 0 && sys_time.minute == 0) 
	{
		today_task.lighting = false;
	}
	if(sys_time.hour == WATTERING_TIME_HOUR && sys_time.minute == WATTERING_TIME_MINUTE) 
	{
		ioport_set_pin_high(DRIVER_ENB);
		ioport_set_pin_high(DRIVER_IN3);
		ioport_set_pin_low(DRIVER_IN4);
		ioport_set_pin_level(LED_BLUE,LOW);
		today_task.wattering = true;
	}
	
	if(sys_time.hour == WATTERING_TIME_HOUR && sys_time.minute == WATTERING_TIME_MINUTE + WATTERING_DURATION)
	{
		ioport_set_pin_low(DRIVER_ENB);
		ioport_set_pin_high(DRIVER_IN3);
		ioport_set_pin_low(DRIVER_IN4);
		ioport_set_pin_level(LED_BLUE,HIGH);
	}
	
}


ISR(RTC_OVF_vect)
{
 	sys_time.second ++;
	 if(sys_time.second == 60)
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
}