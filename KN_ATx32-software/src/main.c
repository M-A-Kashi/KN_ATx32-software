/*
 * Support and FAQ: visit <a href="http://www.kn2c.ir">KN2C Robotic Team</a>
 */
#include <asf.h>

int main (void)
{
	sysclk_init();
	ioport_init();
	
	ioport_set_pin_dir(LED_BLUE, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_GREEN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_WHITE, IOPORT_DIR_OUTPUT);
	
	ioport_configure_pin(BUTTON_0, IOPORT_PULL_UP);
	ioport_configure_pin(BUTTON_1, IOPORT_PULL_UP);
	//board_init();
	
	while(1)
	{
		ioport_toggle_pin_level(LED_GREEN);
		delay_ms(100);
		ioport_set_pin_level(LED_BLUE, ioport_get_pin_level(BUTTON_0));
		ioport_set_pin_level(LED_WHITE, ioport_get_pin_level(BUTTON_1));
	}
}
