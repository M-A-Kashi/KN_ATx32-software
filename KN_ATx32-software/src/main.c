/*
* Support and FAQ: visit <a href="http://www.kn2c.ir">KN2C Robotic Team</a>
*/
#include <asf.h>
#include <stdio.h>
#include <usb_device.h>

#define WATTERING_DURATION 1
#define OPEN  1
#define CLOSE 0
#define WATERING_TIMES 1

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



void wireless_connection ( void );
void valve_manager(void);
void e_valve (uint8_t valve_number, bool state);
void usb_connection(void);
bool timeEqualityCheck(clockTime time1, clockTime time2, bool secondCheck);
watterSchedule watterScheduleCheck(watterSchedule ws);


clockTime sys_time={.hour=10,.minute=47,.second=58};
watterSchedule ws[WATERING_TIMES];

bool valveState[3] = {CLOSE};
bool newSecond = false;

int main (void)
{
	board_init();
	ws[0].openTime.hour = 10;
	ws[0].openTime.minute = 47;
	ws[0].openTime.second = 0;
	ws[0].closeTime.hour   = ws[0].openTime.hour ;
	ws[0].closeTime.minute = ws[0].openTime.minute + WATTERING_DURATION;
	ws[0].closeTime.second = ws[0].openTime.second ;
	ws[0].frequency = 0;
	ws[0].valveNumber = 2;
	
// 	ws[1].openTime.hour = 12;
// 	ws[1].openTime.minute = 2;
// 	ws[1].openTime.second = 15;
// 	ws[1].closeTime.hour = 12;
// 	ws[1].closeTime.minute = 2 + WATTERING_DURATION;
// 	ws[1].closeTime.second = 15;
// 	ws[1].frequency = 0;
// 	ws[1].valveNumber = 2;
	
	while(1)
	{
		delay_ms(20);
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
	for (int i = 0; i < WATERING_TIMES; i++)
	{
		ws[i] = watterScheduleCheck(ws[i]);
	}
	
	// Manual wattering
	if (!ioport_get_pin_level(BUTTON_0))
	{
		e_valve(2,OPEN);
		delay_ms(3);
		while(!ioport_get_pin_level(BUTTON_0));
		e_valve(2,CLOSE);
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
	newSecond = true;
	valve_manager();
	wdt_reset(); 
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
			valveState[1] = OPEN;
			break;
			
			case 2:
			ioport_set_pin_high(DRIVER_ENB);
			ioport_set_pin_high(DRIVER_IN3);
			ioport_set_pin_low(DRIVER_IN4);
			ioport_set_pin_low(LED_BLUE);
			valveState[2] = OPEN;
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
			valveState[1] = CLOSE;
			break;
			
			case 2:
			ioport_set_pin_low(DRIVER_ENB);
			ioport_set_pin_high(DRIVER_IN3);
			ioport_set_pin_low(DRIVER_IN4);
			ioport_set_pin_high(LED_BLUE);
			valveState[2] = CLOSE;
			break;
			
			default:
			break;
		}
	}
	
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

watterSchedule watterScheduleCheck(watterSchedule ws){
	if (timeEqualityCheck(sys_time, ws.openTime, false)){
		e_valve(ws.valveNumber,OPEN);
	}
	if(timeEqualityCheck(sys_time, ws.closeTime, false)){
		e_valve(ws.valveNumber,CLOSE);
	}
	if(timeEqualityCheck(sys_time, ws.closeTime, true)){
		ws.frequency++;
	}
	return ws;
}