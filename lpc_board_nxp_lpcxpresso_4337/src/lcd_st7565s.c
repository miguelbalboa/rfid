/*
 * @brief LPCXpresso Shield LCD functions
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
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
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include <stdlib.h>
#include "board.h"
#include "lcd_st7565s.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#ifndef LCD_FLIP_Y
#define LCD_FLIP_Y  1  /* If 0 Mirrors Y direction */
#endif

/* Font data structure */
static struct {
	int fcolor;
	int bgcolor;
	int spacing;
	int fwidth;
	const FONT_T *font;
} cfont, *cf = &cfont;

static const uint8_t lcd_init_cmd[] = {
	0xA2, /* Set LCD Bias to 1/9 */
	0xA0, /* Set ADC Mode [NORMAL] */
	0xC0 | (LCD_FLIP_Y << 3), /* Set SHL mode */
	0x40, /* Set Start line address as 0 */
	0x2C,
	0x2E,
	0x2F,
	0x25,
	0x81,
	0x20,
	0xAF,  /* Turn LCD ON */
	0xB0,
	0x10,
	0x04,
};

#ifdef LCD_ORIENT_PORTRAIT
static uint8_t fbuffer[LCD_X_RES >> 3][LCD_Y_RES];
#else
static uint8_t fbuffer[LCD_Y_RES >> 3][LCD_X_RES];
#endif

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Write a command to LCD Module */
static void LCD_WriteCmd(const uint8_t *cmd, uint16_t size)
{
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, LCD_CMD_GPIO_PORT, LCD_CMD_GPIO_PIN);
	Board_LCD_WriteData(cmd, size);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, LCD_CMD_GPIO_PORT, LCD_CMD_GPIO_PIN);
}

/* Set column address */
static void LCD_SetRAMAddr(uint8_t pgAddr, uint8_t colAddr)
{
	uint8_t cmd[3];
	cmd[0] = 0xB0 | pgAddr;
	cmd[1] = colAddr & 0x0F; /* Lower addr */
	cmd[2] = 0x10 | (colAddr >> 4); /* Upper addr */
	LCD_WriteCmd(cmd, sizeof(cmd));
}

/* Get the width of a given char index (in pixels) */
static int LCD_GetCharWidth(int index)
{
	if (cf->font->font_width_table) {
		return cf->font->font_width_table[index];
	}
	else {
		return cf->fwidth;
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize and turn on the LCD */
void LCD_Init(void)
{
	int i;

	/* Initialize LCD and turn it On */
	for (i = 0; i < sizeof(lcd_init_cmd); i++) {
		LCD_WriteCmd(&lcd_init_cmd[i], 1);
		if ((lcd_init_cmd[i] >> 3) == 5) {
			volatile uint32_t dly = SystemCoreClock / 100;
			while (dly --);
		}
	}
	LCD_Refresh(0, 0, LCD_X_RES - 1, LCD_Y_RES - 1);
}

/* Refresh the LCD frame buffer content to device */
void LCD_Refresh(int left, int top, int right, int bottom)
{
	int pg;

	/* Sanity check boundaries */
	if (right >= LCD_X_RES) {
		right = LCD_X_RES - 1;
	}

	if (bottom >= LCD_Y_RES) {
		bottom = LCD_Y_RES - 1;
	}

#ifdef LCD_ORIENT_PORTRAIT
	pg = left;
	left = top;
	top = LCD_X_RES - right - 1;
	right = bottom;
	bottom = LCD_X_RES - pg - 1;
#endif
	for (pg = top / 8; pg <= bottom / 8; pg ++) {
		LCD_SetRAMAddr(pg, left);
		Board_LCD_WriteData(&fbuffer[pg][left], right - left + 1);
	}
}

/* Sets a pixel in display RAM */
void LCD_SetPixel(int x, int y, int col)
{
	if (x > LCD_X_RES || y > LCD_Y_RES)
		return;
#ifdef LCD_ORIENT_PORTRAIT
	if (1) {
		int t = x;
		x = y;
		y = LCD_X_RES - t - 1;
	}
#endif
	if (col)
		fbuffer[y/8][x] |= 1 << (y & 7);
	else
		fbuffer[y/8][x] &= ~(1 << (y & 7));
}

/* Sets a pixel in the Display device */
void LCD_PutPixel(int x, int y, int col)
{
	LCD_SetPixel(x, y, col);
	LCD_Refresh(x, y, x, y);
}

/* Draw a rectangle with given coordinates and color */
void LCD_DrawRect(int left, int top, int right, int bottom, int col)
{
	int i;

	for (i = left; i <= right; i++) {
		LCD_SetPixel(i, top, col);
		LCD_SetPixel(i, bottom, col);
	}
	for (i = top + 1; i < bottom; i++) {
		LCD_SetPixel(left, i, col);
		LCD_SetPixel(right, i, col);
	}
	LCD_Refresh(left, top, right, bottom);
}

/* Draw a rectangle filled with given color */
void LCD_FillRect(int left, int top, int right, int bottom, int col)
{
	int i, j;
	/* TODO: improve this algorithm */
	for (i = top; i <= bottom; i++) {
		for (j = left; j <= right; j ++) {
			LCD_SetPixel(j, i, col);
		}
	}
	LCD_Refresh(left, top, right, bottom);
}

/* Draws a line using given cordinates */
void LCD_DrawLine(int x0, int y0, int x1, int y1, int col)
{
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = (dx>dy ? dx : -dy)/2, e2;

	LCD_PutPixel(x0, y0, col);
	while(x0 != x1 || y0 != y1){
		e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
		LCD_PutPixel(x0, y0, col);
	}
}


/* Sets the foreground color of the font */
void LCD_SetFontColor(int color)
{
	cf->fcolor = color;
}

/* Set the background color of the font */
void LCD_SetFontBgColor(int color)
{
	cf->bgcolor = color;
}

/* Set width of the font for fixed width fonts */
void LCD_SetFontWidth(int width)
{
	cf->fwidth = width;
}

/* Sets the current font */
void LCD_SetFont(const FONT_T *font)
{
	cf->font = font;
}

/* Sets the space between two characters */
void LCD_SetFontCharSpace(int space)
{
	cf->spacing = space;
}

/* Prints an ASCII character at a given position */
uint32_t LCD_PutCharXY(int xPos, int yPos, int ch)
{
	int w, h, r, c;
	uint16_t *fp;

	if (!cf->font) {
		return xPos | (yPos << 16);
	}
	if ((ch < cf->font->first_char) || (ch > cf->font->last_char)) {
		return xPos | (yPos << 16);
	}
	ch -= cf->font->first_char;
	w = LCD_GetCharWidth(ch) + cf->spacing;
	h = cf->font->font_height;
	fp = cf->font->font_table + (ch * h);
	for (r = 0; r < h; r++, fp++) {
		uint16_t t = 0x8000;
		for (c = 0; c < w; c ++, t >>= 1) {
			LCD_SetPixel(xPos + c, yPos + r, (*fp & t) ? cf->fcolor : cf->bgcolor);
		}
	}
	LCD_Refresh(xPos, yPos, xPos + w - 1, yPos +  h - 1);
	return (xPos + w) | ((yPos + h) << 16);
}

/* Prints and ASCII string at a given position */
void LCD_PutStrXY(int xPos, int yPos, const char *str)
{
	uint32_t xp = xPos;
	while (str && *str) {
		xp = LCD_PutCharXY(xp & 0xFFFF, yPos, *str++);
	}
}
