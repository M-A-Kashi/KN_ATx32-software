/*
 * Support and FAQ: visit <a href="http://www.kn2c.ir">KN2C Robotic Team</a>
 */
#include <asf.h>
#include <stdio.h>
#include <usb_device.h>

#define WATTERING_DURATION 1
#define OPEN  1
#define CLOSE 0
#define MAX_TIME 1
void wireless_connection ( void );
void clock_1s(void);
void valve_manager(void);
void e_valve (uint8_t valve_number, bool state);
void usb_connection(void);
struct clock_time
{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}sys_time={.hour=16,.minute=35,.second=55},wth[MAX_TIME]; // wth : Wattering Time

bool valve_state[3] = {CLOSE};
uint8_t turn = 0;
int test;
bool usb_new = false;


int main (void)
{
	board_init();
	wth[0].hour = 12;  //wth[1].hour = 15;
	wth[0].minute = 0;//wth[1].minute = 0;
	
	while(1)
	{		
		if (turn < MAX_TIME)
		{
			valve_manager();
		}
		delay_ms(20);
		usb_connection();
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



void valve_manager (void)
{
	if(sys_time.hour == 0 && sys_time.minute == 0) 
	{
		turn = 0;
	}
	
	if((sys_time.hour == wth[turn].hour) && (sys_time.minute == wth[turn].minute) && !valve_state[2]) 
	{
		e_valve(2,OPEN);
		test ++;
	}
	
	if((sys_time.hour == wth[turn].hour) && (sys_time.minute == (wth[turn].minute + WATTERING_DURATION)) && valve_state[2])
	{
		e_valve(2,CLOSE);
		turn ++ ;
	}
	
	// Manual wattering
	if (!ioport_get_pin_level(BUTTON_0))
	{
		e_valve(2,OPEN);
		ioport_set_pin_level(LED_BLUE,LOW);
		delay_ms(3);
		while(!ioport_get_pin_level(BUTTON_0));
		e_valve(2,CLOSE);
		ioport_set_pin_level(LED_BLUE,HIGH);
	}
	
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
	usb_new = true;
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
			valve_state[1] = OPEN;
			break;
			
			case 2:
			ioport_set_pin_high(DRIVER_ENB);
			ioport_set_pin_high(DRIVER_IN3);
			ioport_set_pin_low(DRIVER_IN4);
			ioport_set_pin_low(LED_BLUE);
			valve_state[2] = OPEN;
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
			valve_state[1] = CLOSE;
			break;
			
			case 2:
			ioport_set_pin_low(DRIVER_ENB);
			ioport_set_pin_high(DRIVER_IN3);
			ioport_set_pin_low(DRIVER_IN4);
			ioport_set_pin_high(LED_BLUE);
			valve_state[2] = CLOSE;
			break;
			
			default:
			break;
		}
	}
	
}

void usb_connection(void)
{
	if (udd_is_underflow_event() && usb_new)
	{
		char usb_out [100];
		uint8_t count = sprintf(usb_out, "TIME : %d:%d:%d    %d  %d   times of wattering: %d \r",sys_time.hour,sys_time.minute,sys_time.second,RTC.CNT,turn,test);
		for (int i=0;i<count;i++)
		{
			udi_cdc_putc(usb_out[i]);
		}
		usb_new = false;
	}
	
}