//******************************************************************************
//*
//*     FULLNAME:  LCD low level driver. HD44780 controller
//*
//*     FILENAME:  lcdhw.h
//*
//*     PROCESSOR: ARM Cortex-M3 
//*
//*     $Id$
//*
//*     Copyright (c) 2009-2010, Ivan A-R <ivan@tuxotronic.org>
//*
//*     Permission is hereby granted, free of charge, to any person 
//*     obtaining  a copy of this software and associated documentation 
//*     files (the "Software"), to deal in the Software without restriction, 
//*     including without limitation the rights to use, copy, modify, merge, 
//*     publish, distribute, sublicense, and/or sell copies of the Software, 
//*     and to permit persons to whom the Software is furnished to do so, 
//*     subject to the following conditions:
//*
//*     The above copyright notice and this permission notice shall be included 
//*     in all copies or substantial portions of the Software.
//*
//*     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
//*     EXPRESS  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
//*     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
//*     IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
//*     CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN cc OF CONTRACT, 
//*     TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH 
//*     THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//*
//******************************************************************************

#ifndef _LCDHW_H_
#define _LCDHW_H_

#include <ch.h>
#include <hal.h>
#include "board.h"
#include "lcd_encode.h"
#include "utils.h"
#include "lcdhwcfg.h"


/*
 * Forward declarations
 */

static void _lcd_init(void);
static void _lcdBackLight(lcdbrightness_t light);

/**
 * Send direct command to LCD
 */
static void lcdCmd(unsigned char cmd);

/**
 * Print char to display
 */
static void lcdData(unsigned char data);

/**
 * Print encoded char to display
 */
#define lcdEncData(data) lcdData(encodeLcd(data));


/**
 * LCD controller commands
 */
#define LCD_CLEAR               0x01

#define LCD_RETURN_HOME         0x02

#define LCD_SET_INCREMENT_MODE  0x06

#define LCD_SET_DM              0x08
#define LCD_DM_DISPLAY_ON       4
#define LCD_DM_DISPLAY_OFF      0
#define LCD_DM_CURSOR_ON        2
#define LCD_DM_CURSOR_OFF       0
#define LCD_DM_BLINK_ON         1
#define LCD_DM_BLINK_OFF        0

#define LCD_CMD_INIT4           0x28
#define LCD_CMD_INIT8           0x38

#define LCD_SET_DDRAM_ADDRESS   0x80

#define LCD_LINE1               0
#define LCD_LINE2               40



/**
 * Controller depend part
 */
static void _lcdSetCursor(lcdcursor_t cursor)
{
    switch(cursor)
    {
        case lctBLOCK:
            lcdCmd(LCD_SET_DM | LCD_DM_DISPLAY_ON | LCD_DM_CURSOR_ON);
            break;
        case lctBLINK:
        case lctUNDERLINE:
            lcdCmd(LCD_SET_DM | LCD_DM_DISPLAY_ON | LCD_DM_CURSOR_ON | LCD_DM_BLINK_ON);
            break;
        default:
            lcdCmd(LCD_SET_DM | LCD_DM_DISPLAY_ON);
    }
}

static void _lcdGoto(unsigned char row, unsigned char col)
{
    char line_base;
    switch(row)
    {
        default:
            line_base = 0x00;
            break;
        case 1:
            line_base = 0x40;
            break;
        case 2:
            line_base = 0x14;
            break;
        case 3:
            line_base = 0x54;
            break;
    }
    lcdCmd((line_base + col) | LCD_SET_DDRAM_ADDRESS);
}

static void _lcdHome(void)
{
    lcdCmd(LCD_RETURN_HOME);
}

static void _lcdClr(void)
{
    lcdCmd(LCD_CLEAR);
    lcdCmd(LCD_RETURN_HOME);
}


/**
 * GPIO depend part
 */
#define LCD_GPIO GPIOC
#define LCD_RS (1<<4)
#define LCD_E  (1<<5)

#define LCD_DSHIFT 0
#define LCD_D0 (1<<(LCD_DSHIFT+0))
#define LCD_D1 (1<<(LCD_DSHIFT+1))
#define LCD_D2 (1<<(LCD_DSHIFT+2))
#define LCD_D3 (1<<(LCD_DSHIFT+3))
#define LCD_DMASK (LCD_D0 | LCD_D1 | LCD_D2 | LCD_D3)

#define LCD_BL (1<<0)


#define LCD_E_MARK() LCD_GPIO->BSRR = LCD_E
#define LCD_E_RELEASE() LCD_GPIO->BRR = LCD_E

#define LCD_RS0() LCD_GPIO->BRR = LCD_RS
#define LCD_RS1() LCD_GPIO->BSRR = LCD_RS

#define LCD_DATA(x) { LCD_GPIO->BRR = LCD_DMASK; LCD_GPIO->BSRR = ((x)<<LCD_DSHIFT) & LCD_DMASK; }

#define LCD_PINS_ZERO() LCD_GPIO->BRR = (LCD_DMASK | LCD_RS | LCD_E)

#define LCD_BL_ON()  GPIOB->BSRR = LCD_BL
#define LCD_BL_OFF() GPIOB->BRR  = LCD_BL

/*
 * Hardware depend commands
 */

//#define E_DELAY 10
#define E_DELAY 10
#define LCD_E_DELAY() { volatile unsigned char i; for(i=0; i<E_DELAY; i++) ; }
#define LCD_SLEEP_MS(ms) chThdSleep((systime_t)(ms))

static void _lcdBackLight(lcdbrightness_t light)
{
    if(light) LCD_BL_ON();
         else LCD_BL_OFF();
}

static inline void pulse_e(void)
{
    LCD_E_MARK();
    LCD_E_DELAY();
    LCD_E_RELEASE();
    LCD_E_DELAY(); // ???
}

static inline void wait_ready(void)
{
    LCD_SLEEP_MS(1);
}


static inline void _lcd_init(void)
{
    // GPIO must be already configured

    LCD_PINS_ZERO();
    LCD_SLEEP_MS(16); // 16 ms

    LCD_DATA(3); // 8bit mode
    pulse_e();
    LCD_SLEEP_MS(5); // 4992 us

    pulse_e(); // Repeat 8bit mode
    LCD_SLEEP_MS(1); // 64 us

    pulse_e(); // Third repeat
    LCD_SLEEP_MS(1); // 64 us

    LCD_GPIO->BRR = LCD_D3 | LCD_D2 | LCD_D1 | LCD_D0;
    LCD_GPIO->BSRR = (0x02)<<0; // 4bit mode reconfigure
    LCD_DATA(2); // Change to 4bit mode
    pulse_e();

    lcdCmd(LCD_CMD_INIT4);
    /*
    lcdCmd(0x24);
    lcdCmd(0x09);
    lcdCmd(0x20);
    */

    lcdCmd(0x08);
    lcdCmd(0x01);
    lcdCmd(0x06);
    lcdCmd(0x0C);

    lcdCmd(0x80);
    /*
    lcdCmd(0x02);
    */
    /*
    lcdCmd(LCD_SET_DM | LCD_DM_DISPLAY_ON);
    lcdCmd(LCD_SET_INCREMENT_MODE);
    */
}

static void lcdCmd(unsigned char cmd)
{
    wait_ready();
    LCD_RS0(); // controll command
    LCD_DATA(cmd >> 4);
    pulse_e();
    LCD_DATA(cmd & 0x0F);
    pulse_e();
}

static void lcdData(unsigned char data)
{
    wait_ready();
    LCD_RS1(); // data
    LCD_DATA(data >> 4);
    pulse_e();
    LCD_DATA(data & 0x0F);
    pulse_e();
}

#endif /* _LCDHW_H_ */

