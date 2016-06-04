/*
 * Support and FAQ: visit <a href="http://www.kn2c.ir">KN2C Robotic Team</a>
 */
#include <asf.h>
#include <stdio.h>
void wireless_connection ( void );
int main (void)
{
	board_init();

	while(1)
	{
		delay_ms(20);
		ioport_set_pin_level(LED_WHITE,HIGH);
		ioport_set_pin_level(LED_BLUE,HIGH);
		

		char usb_out [100];
		uint8_t count = sprintf(usb_out, "TIME : %ld \r",rtc_get_time());
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
}