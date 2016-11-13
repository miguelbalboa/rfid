/*
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "string.h"

#include "retarget.h"
#include "wm8904.h"

/** @ingroup BOARD_NGX_XPLORER_18304330
 * @{
 */

/* SDIO Data pin configuration bits */
#define SDIO_DAT_PINCFG (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_PULLUP | SCU_MODE_FUNC7)

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* System configuration variables used by chip driver */
const uint32_t ExtRateIn = 0;
const uint32_t OscRateIn = 12000000;

typedef struct {
	uint8_t port;
	uint8_t pin;
} io_port_t;

static const io_port_t gpioLEDBits[] = {{3, 5}, {0, 7}, {3, 7}};
static uint32_t lcd_cfg_val;

void Board_UART_Init(LPC_USART_T *pUART)
{
	Chip_SCU_PinMuxSet(0x6, 4, (SCU_MODE_INACT | SCU_MODE_FUNC2));					/* P6,4 : UART0_TXD */
	Chip_SCU_PinMuxSet(0x2, 1, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC1));/* P2.1 : UART0_RXD */

	if(pUART == LPC_USART3){
	Chip_SCU_PinMuxSet(0x2, 3, (SCU_MODE_INACT | SCU_MODE_FUNC2));					/* P2_3 : UART3_TXD */
	Chip_SCU_PinMuxSet(0x2, 4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC2));/* P2_4 : UART3_RXD */
	}
}

/* Initialize debug output via UART for board */
void Board_Debug_Init(void)
{
#if defined(DEBUG_UART)
	Board_UART_Init(DEBUG_UART);

	Chip_UART_Init(DEBUG_UART);
	Chip_UART_SetBaudFDR(DEBUG_UART, 115200);
	Chip_UART_ConfigData(DEBUG_UART, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);

	/* Enable UART Transmit */
	Chip_UART_TXEnable(DEBUG_UART);
#endif
}

/* Sends a character on the UART */
void Board_UARTPutChar(char ch)
{
#if defined(DEBUG_UART)
	/* Wait for space in FIFO */
	while ((Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_THRE) == 0) {}
	Chip_UART_SendByte(DEBUG_UART, (uint8_t) ch);
#endif
}

/* Gets a character from the UART, returns EOF if no character is ready */
int Board_UARTGetChar(void)
{
#if defined(DEBUG_UART)
	if (Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_RDR) {
		return (int) Chip_UART_ReadByte(DEBUG_UART);
	}
#endif
	return EOF;
}

/* Outputs a string on the debug UART */
void Board_UARTPutSTR(const char *str)
{
#if defined(DEBUG_UART)
	while (*str != '\0') {
		Board_UARTPutChar(*str++);
	}
#endif
}

static void Board_LED_Init()
{
	uint32_t idx;

	for (idx = 0; idx < (sizeof(gpioLEDBits) / sizeof(io_port_t)); ++idx) {
		/* Set pin direction and init to off */
		Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, gpioLEDBits[idx].port, gpioLEDBits[idx].pin);
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, gpioLEDBits[idx].port, gpioLEDBits[idx].pin, (bool) true);
	}
}

void Board_LED_Set(uint8_t LEDNumber, bool On)
{
	if (LEDNumber < (sizeof(gpioLEDBits) / sizeof(io_port_t)))
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, gpioLEDBits[LEDNumber].port, gpioLEDBits[LEDNumber].pin, (bool) !On);
}

bool Board_LED_Test(uint8_t LEDNumber)
{
	if (LEDNumber < (sizeof(gpioLEDBits) / sizeof(io_port_t)))
		return (bool) !Chip_GPIO_GetPinState(LPC_GPIO_PORT, gpioLEDBits[LEDNumber].port, gpioLEDBits[LEDNumber].pin);

	return false;
}

void Board_LED_Toggle(uint8_t LEDNumber)
{
	Board_LED_Set(LEDNumber, !Board_LED_Test(LEDNumber));
}

void Board_Buttons_Init(void)
{
	Chip_SCU_PinMuxSet(0x2, 7, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));		// P2_7 as GPIO0[7]
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BUTTONS_BUTTON1_GPIO_PORT_NUM, BUTTONS_BUTTON1_GPIO_BIT_NUM);	// input
}

uint32_t Buttons_GetStatus(void)
{
	uint8_t ret = NO_BUTTON_PRESSED;
	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, BUTTONS_BUTTON1_GPIO_PORT_NUM, BUTTONS_BUTTON1_GPIO_BIT_NUM) == 0) {
		ret |= BUTTONS_BUTTON1;
	}
	return ret;
}

void Board_Joystick_Init(void)
{}

uint8_t Joystick_GetStatus(void)
{
	return NO_BUTTON_PRESSED;
}

/* Returns the MAC address assigned to this board */
void Board_ENET_GetMacADDR(uint8_t *mcaddr)
{
	uint8_t boardmac[] = {0x00, 0x60, 0x37, 0x12, 0x34, 0x56};

	memcpy(mcaddr, boardmac, 6);
}

/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
	/* Sets up DEBUG UART */
	DEBUGINIT();

	/* Initializes GPIO */
	Chip_GPIO_Init(LPC_GPIO_PORT);

	/* Initialize LEDs */
	Board_LED_Init();
	Chip_ENET_RMIIEnable(LPC_ETHERNET);
}

void Board_I2C_Init(I2C_ID_T id)
{
	if (id == I2C1) {
		/* Configure pin function for I2C1*/
		Chip_SCU_PinMuxSet(0x2, 3, (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC1));		/* P2.3 : I2C1_SDA */
		Chip_SCU_PinMuxSet(0x2, 4, (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC1));		/* P2.4 : I2C1_SCL */
	}
	else {
		Chip_SCU_I2C0PinConfig(I2C0_STANDARD_FAST_MODE);
	}
}

void Board_SDMMC_Init(void)
{
	Chip_SCU_PinMuxSet(0x1, 9, SDIO_DAT_PINCFG);	/* P1.9 connected to SDIO_D0 */
	Chip_SCU_PinMuxSet(0x1, 10, SDIO_DAT_PINCFG);	/* P1.10 connected to SDIO_D1 */
	Chip_SCU_PinMuxSet(0x1, 11, SDIO_DAT_PINCFG);	/* P1.11 connected to SDIO_D2 */
	Chip_SCU_PinMuxSet(0x1, 12, SDIO_DAT_PINCFG);	/* P1.12 connected to SDIO_D3 */

	Chip_SCU_ClockPinMuxSet(2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC4));	/* CLK2 connected to SDIO_CLK */
	Chip_SCU_PinMuxSet(0x1, 6, SDIO_DAT_PINCFG);	/* P1.6 connected to SDIO_CMD */
	Chip_SCU_PinMuxSet(0x1, 13, (SCU_MODE_INBUFF_EN | SCU_MODE_FUNC7));	/* P1.13 connected to SDIO_CD */
}

void Board_SSP_Init(LPC_SSP_T *pSSP)
{
	if (pSSP == LPC_SSP1) {
		Chip_SCU_PinMuxSet(0x1, 5, (SCU_PINIO_FAST | SCU_MODE_FUNC5));  /* P1.5 => SSEL1 */
		Chip_SCU_PinMuxSet(0xF, 4, (SCU_PINIO_FAST | SCU_MODE_FUNC0));  /* PF.4 => SCK1 */

		Chip_SCU_PinMuxSet(0x1, 4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5)); /* P1.4 => MOSI1 */
		Chip_SCU_PinMuxSet(0x1, 3, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5)); /* P1.3 => MISO1 */
	}
	else if(pSSP == LPC_SSP0){
		//P3_0 => SCK0
		Chip_SCU_PinMuxSet(0x3, 0, (SCU_PINIO_FAST | SCU_MODE_FUNC4));
		//P1_0 => SSEL0
		Chip_SCU_PinMuxSet(0x1, 0, (SCU_PINIO_FAST | SCU_MODE_FUNC5));
		//P1_1 => MISO0
		Chip_SCU_PinMuxSet(0x1, 1, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5));
		//P1_2 -> MOSI2
		Chip_SCU_PinMuxSet(0x1, 2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5));
	}
	else {
		return;
	}
}

/* Initialize DAC interface for the board */
void Board_DAC_Init(LPC_DAC_T *pDAC)
{
	Chip_SCU_DAC_Analog_Config();
}

/* Initialize Audio Codec */
static Status Board_Audio_CodecInit(int micIn)
{

	if (!WM8904_Init(micIn)){
		return ERROR;
	}

	return SUCCESS;
}

/* Board Audio initialization */
void Board_Audio_Init(LPC_I2S_T *pI2S, int micIn)
{

	if (pI2S == LPC_I2S0) {
		/* TODO :Add pin mux for I2S0 later */
	} else if (pI2S == LPC_I2S1) {
		Chip_SCU_PinMuxSet (0x1, 19, (SCU_PINIO_FAST | SCU_MODE_FUNC7)); /* I2S1_TX_SCK */
		Chip_SCU_PinMuxSet (0x0, 1,  (SCU_PINIO_FAST | SCU_MODE_FUNC7)); /* I2S1_TX_SDA */
		Chip_SCU_PinMuxSet (0x3, 4,  (SCU_PINIO_FAST | SCU_MODE_FUNC6)); /* I2S1_RX_SDA */
		Chip_SCU_PinMuxSet (0x0, 0,  (SCU_PINIO_FAST | SCU_MODE_FUNC7)); /* I2S1_TX_WS */

        Chip_SCU_ClockPinMuxSet(0,(SCU_MODE_FUNC1|SCU_MODE_INACT));
		/* Setup base clock for CLKOUT */
		Chip_Clock_SetBaseClock(CLK_BASE_OUT, CLKIN_CRYSTAL, false, false);
	} else {
		/* It is a BUG catch it */
		while(1);
	}
	/* Init WM8904 CODEC */
	while (Board_Audio_CodecInit(micIn) != SUCCESS) {}
}

/* Initialize Pin Muxing for LCD */
void Board_LCD_Init(void)
{
	uint32_t val;

	Board_SSP_Init(LCD_SSP);
	val = LCD_SSP->CR0 & 0xFFFF;
	Chip_SCU_PinMuxSet(LCD_CDM_PORT, LCD_CMD_PIN, LCD_CMD_CFG); //P6_5 = GPIO3[4]
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, LCD_CMD_GPIO_PORT, LCD_CMD_GPIO_PIN);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, LCD_CMD_GPIO_PORT, LCD_CMD_GPIO_PIN);

	/* Enable the SSP interface */
	Chip_SSP_Init(LCD_SSP);
	Chip_SSP_Set_Mode(LCD_SSP, SSP_MODE_MASTER);
	Chip_SSP_SetFormat(LCD_SSP, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA1_CPOL1);
	Chip_SSP_SetBitRate(LCD_SSP, LCD_BIT_RATE);
	Chip_SSP_Enable(LCD_SSP);

	lcd_cfg_val = LCD_SSP->CR0 & 0xFFFF;
	LCD_SSP->CR0 = val;
}

/* Write data to LCD module */
void Board_LCD_WriteData(const uint8_t *data, uint16_t size)
{
	uint32_t val = LCD_SSP->CR0 & 0xFFFF;
	LCD_SSP->CR0 = lcd_cfg_val;
	Chip_SSP_WriteFrames_Blocking(LCD_SSP, data, size);
	LCD_SSP->CR0 = val;
}

void Board_LCD_Chip_Select_Down(){
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, LCD_CMD_GPIO_PORT, LCD_CMD_GPIO_PIN);
}
/**
 * @}
 */
