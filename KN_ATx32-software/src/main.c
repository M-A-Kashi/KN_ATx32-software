/*
 * Support and FAQ: visit <a href="http://www.kn2c.ir">KN2C Robotic Team</a>
 */
#include <asf.h>
void force_boot_loader(void);

int main (void)
{
	sysclk_init();
	ioport_init();
	
	ioport_set_pin_dir(LED_BLUE, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_GREEN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_WHITE, IOPORT_DIR_OUTPUT);
	
	ioport_configure_pin(BUTTON_0, IOPORT_PULL_UP);
	ioport_configure_pin(BUTTON_1, IOPORT_PULL_UP);

	force_boot_loader();
	
	irq_initialize_vectors();
	cpu_irq_enable();
	udc_start();
	//board_init();

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


void force_boot_loader(void)
{
	if ( !ioport_get_pin_level(BUTTON_1) |  !ioport_get_pin_level(BUTTON_0))
	{
		goto *(BOOT_SECTION_START + 0x1FC) ;
	}
}


