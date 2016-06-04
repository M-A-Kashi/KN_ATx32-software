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
void spi_init_pins(void);
void spi_init_module(void);
void nrf_init (void);
void module_id_set(void);
char Address[_Address_Width] = { 0x11, 0x22, 0x33, 0x44, 0x55};


void board_init(void)
{
	// Initialize the PMIC
	pmic_init();
	// System Clock Initialization
	sysclk_init();
	// Initializes GPIO
	ioport_init();
	ioport_configure_pin(LED_BLUE, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED_GREEN, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED_WHITE, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(BUTTON_0, IOPORT_PULL_UP);
	ioport_configure_pin(BUTTON_1, IOPORT_PULL_UP);
	spi_init_pins();
	// Boot-loader setting
	force_boot_loader();
	// RTC setting
	// Instead of configuring conf_clock. h use these settings to avoid corrupting USB settings
	uint8_t id = SYSCLK_RTCSRC_RCOSC;
	CLK.RTCCTRL = id | CLK_RTCEN_bm;
	rtc_init();
	// setting period for overflow interrupt after 1s
	while(RTC.STATUS & RTC_SYNCBUSY_bm)
	RTC.PER = 1;
	RTC.CNT = 0;
	// USB settings
	irq_initialize_vectors();
	cpu_irq_enable();
	udc_start();
	//SPI settings
	spi_init_module();
	module_id_set();
	nrf_init();
}

void force_boot_loader(void)
{
	if ( !ioport_get_pin_level(BUTTON_1) |  !ioport_get_pin_level(BUTTON_0))
	{
		goto *(BOOT_SECTION_START + 0x1FC) ;
	}
}

void spi_init_pins(void)
{	
	ioport_configure_pin(IRQ,IOPORT_DIR_INPUT|IOPORT_SENSE_FALLING|PORT_OPC_PULLUP_gc);
	ioport_configure_pin(CE,IOPORT_DIR_OUTPUT);
	ioport_configure_pin(CSN,IOPORT_DIR_OUTPUT);
	ioport_configure_pin(MOSI,IOPORT_DIR_OUTPUT);
	ioport_configure_pin(MISO,IOPORT_DIR_INPUT);
	ioport_configure_pin(SCK,IOPORT_DIR_OUTPUT);
	PORTC.INTCTRL  = PORT_INT0LVL_HI_gc ;
	PORTC.INT0MASK = ioport_pin_to_mask(IRQ);
}

void spi_init_module(void)
{
	struct spi_device spi_device_conf = {
		.id = CSN
	};
	spi_master_init(&SPIC);
	spi_master_setup_device(&SPIC, &spi_device_conf, SPI_MODE_0, 8000000UL, 0);
	spi_enable(&SPIC);
}

void nrf_init (void)
{

	delay_ms(11);
	NRF24L01_Clear_Interrupts();
	NRF24L01_Flush_TX();
	NRF24L01_Flush_RX();
	NRF24L01_Init_milad(_RX_MODE, _CH_1, _2Mbps, Address, _Address_Width, _Buffer_Size, RF_PWR_MAX);
	NRF24L01_WriteReg(W_REGISTER | EN_AA, 0x01);
	NRF24L01_WriteReg(W_REGISTER | DYNPD,0x01);
	NRF24L01_WriteReg(W_REGISTER | FEATURE,0x06);

	NRF24L01_CE_HIGH;//rx mode  ?
	delay_us(130);
}

void module_id_set(void)
{
	Address[4] = (MODULE_ID << 4 ) | MODULE_ID ;
}