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
void watchDogInit(void);
void adc_init(void);


void board_init(void)
{
	// Initialize the PMIC
	pmic_init();
	// System Clock Initialization
	sysclk_init();
	// Initializes GPIO
	ioport_init();
	
	adc_init();
	
	ioport_configure_pin(LED_BLUE, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED_GREEN, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED_WHITE, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(BUTTON_0, IOPORT_PULL_UP);
	ioport_configure_pin(BUTTON_1, IOPORT_PULL_UP);
	
	ioport_configure_pin(DRIVER_IN1, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(DRIVER_IN2, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(DRIVER_IN3, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(DRIVER_IN4, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(DRIVER_ENA, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(DRIVER_ENB, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	
	//Temperature sensor
	ioport_configure_pin(PA5, IOPORT_PULL_DOWN);
	spi_init_pins();
	// Boot-loader setting
	//force_boot_loader();
	// RTC setting
	// Instead of configuring conf_clock. h use these settings to avoid corrupting USB settings
	uint8_t id = SYSCLK_RTCSRC_RCOSC;
	CLK.RTCCTRL = id | CLK_RTCEN_bm;
	rtc_init();
	// setting period for overflow interrupt after 1s
	while(RTC.STATUS & RTC_SYNCBUSY_bm)
	RTC.PER = 1023;
	RTC.CNT = 0;
	// USB settings
	irq_initialize_vectors();
	cpu_irq_enable();
	udc_start();
	//SPI settings
	spi_init_module();
	watchDogInit();
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



void watchDogInit(){
	wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_4KCLK);
	wdt_enable();
}

void adc_init(void)
{
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;
	adc_read_configuration(&LM35_ADC, &adc_conf);
	adcch_read_configuration(&LM35_ADC, LM35_ADC_CH, &adcch_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12, ADC_REF_BANDGAP);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN5, ADCCH_NEG_PAD_GND, 1);
	adc_write_configuration(&LM35_ADC, &adc_conf);
	adcch_write_configuration(&LM35_ADC, LM35_ADC_CH, &adcch_conf);
	adc_enable(&LM35_ADC);
}