/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>
void force_boot_loader(void);

void board_init(void)
{
	pmic_init();
	sysclk_init();
	ioport_init();
	
	ioport_set_pin_dir(LED_BLUE, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_GREEN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_WHITE, IOPORT_DIR_OUTPUT);
	
	ioport_configure_pin(BUTTON_0, IOPORT_PULL_UP);
	ioport_configure_pin(BUTTON_1, IOPORT_PULL_UP);

	force_boot_loader();
	
	// Instead of configuring conf_clock. h use these settings to avoid corrupting USB settings
	uint8_t id = SYSCLK_RTCSRC_RCOSC;
	CLK.RTCCTRL = id | CLK_RTCEN_bm;
	
	rtc_init();
	
	irq_initialize_vectors();
	cpu_irq_enable();
	udc_start();
}

void force_boot_loader(void)
{
	if ( !ioport_get_pin_level(BUTTON_1) |  !ioport_get_pin_level(BUTTON_0))
	{
		goto *(BOOT_SECTION_START + 0x1FC) ;
	}
}

