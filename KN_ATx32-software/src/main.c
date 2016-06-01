/*
 * Support and FAQ: visit <a href="http://www.kn2c.ir">KN2C Robotic Team</a>
 */
#include <asf.h>


int main (void)
{
	board_init();

	while(1)
	{
		ioport_toggle_pin_level(LED_GREEN);
		delay_ms(100);
		ioport_set_pin_level(LED_BLUE, ioport_get_pin_level(BUTTON_0));
		ioport_set_pin_level(LED_WHITE, ioport_get_pin_level(BUTTON_1));
		
		char usb_in = udi_cdc_getc();
		char usb_out [17]=  "WHAT YOU TYPED: \r";//udi_cdc_getc();
		for (int i=0;i<16;i++)
		{
			udi_cdc_putc(usb_out[i]);
		}
		udi_cdc_putc(usb_in);
		udi_cdc_putc('\r');
	}
}