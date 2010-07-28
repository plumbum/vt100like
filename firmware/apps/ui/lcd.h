//******************************************************************************
//*
//*     FULLNAME:  LCD abstract interface
//*
//*     FILENAME:  lcd.h
//*
//*     PROCESSOR: not specified
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
//*     CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
//*     TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH 
//*     THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//*
//******************************************************************************

#ifndef _LCD_H_
#define _LCD_H_

#include <inttypes.h>
#include <config.h>

/**
 * Use VTxxx emulator extensions
 * Must be defined in config.h
 * I.e. #define LCD_USE_VT 1
 */
#ifndef LCD_USE_VT
#define LCD_USE_VT 0
#endif

#ifndef EOLN
#define EOLN "\r\n"
#endif

typedef unsigned char lcdpos_t;
typedef unsigned char lcdbrightness_t;
typedef enum {lctOFF=0, lctBLOCK, lctBLINK, lctUNDERLINE, lctHALF} lcdcursor_t;
typedef enum {ldtOFF=0, ldtON=!ldtOFF} lcddisplay_t;

#include "lcdhw.h"

#ifdef LCD_COLOR
typedef unsigned char lcdcolor_t;
#endif /* LCD_COLOR */

typedef struct {
    lcdpos_t row;
    lcdpos_t col;
    lcdpos_t frame_row1;
    lcdpos_t frame_row2;
    lcdpos_t tabsize;
    lcdcursor_t cursor;     /* Cursor type */
#if (LCD_COLOR == 1)
    lcdcolor_t fg;          /* Foreground symbol color */
    lcdcolor_t bg;          /* Background symbol color */
#if (LCD_USE_VT == 1)
    lcdcolor_t saved_fg;
    lcdcolor_t saved_bg;
#endif /* LCD_USE_VT */
#endif /* LCD_COLOR */
    lcdbrightness_t light;  /* Backlight brighthess */
    int inverse:1;          /* Font style: invese */
    int bold:1;             /* Font sytle: bold */
    int wrap:1;             /* Enable line wrap */
    int scroll:1;           /* Enable scroll display */
    int needrepos:1;        /* Internal flag. Do not change */
#if (LCD_USE_VT == 1)
    lcdpos_t saved_row;
    lcdpos_t saved_col;
    signed char fmstate;    /* Can use by finite-state machine. I.e. for VT100 terminal emulation */
#endif /* LCD_USE_VT */
    char dbuf[LCD_ROWS][LCD_COLS];      /* Double buffer */
} lcdenv_t;

#ifdef DEFAULT_WORKENV
extern lcdenv_t* workenv;
#endif

/**
 * LCD initialization routine
 */
void lcdInit(lcdenv_t* env);

/**
 * Force LCD hardware init
 * Not recomended to use
 */
void lcdHwInit(void);


/**
 * Init new display environment
 */
void lcdInitEnv(lcdenv_t* env);

/**
 * Free display environment
 */
void lcdFreeEnv(lcdenv_t* env);

/**
 * Set current displayed environment
 * Refresh if needed
 */
void lcdSetDispEnv(lcdenv_t* env);

/**
 * Get current displayed environment
 */
lcdenv_t* lcdGetDispEnv(void);

#ifdef DEFAULT_WORKENV
#define lcdSetWorkEnv(env) envwork = env
#define lcdGetWorkEnv() (env)
#endif


/**
 * Refres current display environment
 * @param row1 - first refreshing line
 * @param row2 - last refreshing line
 */
void lcdScreenRefresh(lcdpos_t row1, lcdpos_t row2);

//lcdpos_t lcdGetRows(void);
#define lcdGetRows() LCD_ROWS
//lcdpos_t lcdGetCols(void);
#define lcdGetCols() LCD_COLS

/**
 * Get current row
 * @param env - work environment
 */
lcdpos_t lcdGetRow(lcdenv_t* env);
#define lcdwGetRow() lcdGetRow(workenv);

/**
 * Get current column
 * @param env - work environment
 */
lcdpos_t lcdGetCol(lcdenv_t* env);
#define lcdwGetCol() lcdGetCol(workenv);

/**
 * Move cursor
 * @param env - work environment
 * @param row 
 * @param col 
 */
void lcdGoto(lcdenv_t* env, lcdpos_t row, lcdpos_t col);
#define lcdwGoto(row, col) lcdGoto(workenv, row, col)

/**
 * Set cursor type
 * @param env - work environment
 * @param cursor - cursor type
 */
void lcdSetCursor(lcdenv_t* env, lcdcursor_t cursor);
#define lcdwSetCursor(cursor) lcdSetCursor(workenv, cursor)


/**
 * Return cursor to upper left position
 * @param env - work environment
 */
void lcdHome(lcdenv_t* env);
#define lcdwHome() lcdHome(workenv)

/**
 * Return cursor to left position of current line
 * @param env - work environment
 */
void lcdHomeLine(lcdenv_t* env);
#define lcdwHomeLine() lcdHomeLine(workenv)

/**
 * Set cursor at last position of current line
 * @param env - work environment
 */
void lcdEndLine(lcdenv_t* env);
#define lcdwEndLine() lcdEndLine(workenv)

/**
 * Tabulation
 * Clean tabulated space
 * @param env - work environment
 */
void lcdTab(lcdenv_t* env);
#define lcdwTab() lcdTab(workenv)

/**
 * Move cursor left
 * @param env - work environment
 * @param size - positions to skip
 */
void lcdLeft(lcdenv_t* env, lcdpos_t size);
#define lcdwLeft(size) lcdLeft(workenv, size)

/**
 * Move cursor right
 * @param env - work environment
 * @param size - positions to skip
 */
void lcdRight(lcdenv_t* env, lcdpos_t size);
#define lcdwRight(size) lcdRight(workenv, size)

/**
 * Move cursor up
 * @param env - work environment
 * @param size - positions to skip
 */
void lcdUp(lcdenv_t* env, lcdpos_t size);
#define lcdwUp(size) lcdUp(workenv, size)

/**
 * Move cursor down
 * @param env - work environment
 * @param size - positions to skip
 */
void lcdDown(lcdenv_t* env, lcdpos_t size);
#define lcdwDown(size) lcdDown(workenv, size)

void lcdLineFeed(lcdenv_t* env);
#define lcdwLineFeed() lcdLineFeed(workenv)

void lcdSetCol(lcdenv_t* env, lcdpos_t col);
#define lcdwSetCol(col) lcdSetCol(workenv, col)

void lcdClr(lcdenv_t* env);
#define lcdwClr() lcdClr(workenv)

void lcdClrLine(lcdenv_t* env, lcdpos_t row);
#define lcdwClrLine(row) lcdClrLine(workenv, row)

void lcdClrCols(lcdenv_t* env, lcdpos_t row, lcdpos_t colfrom, lcdpos_t colto);
#define lcdwClrCols(row, colfrom, colto) lcdClrCols(workenv, row, colfrom, colto)

#define lcdClrEndl(env) lcdClrCols((env), lcdGetRow(env), lcdGetCol(env), lcdGetCols()-1)
#define lcdwClrEndl() lcdClrCols(workenv, lcdGetRow(workenv), lcdGetCol(workenv), lcdGetCols()-1)

void lcdScreenScrollUp(lcdenv_t* env, lcdpos_t row1, lcdpos_t row2);
#define lcdwScreenScrollUp(row1, row2) lcdScreenScrollUp(workenv, row1, row2)

void lcdBackLight(lcdenv_t* env, lcdbrightness_t light);
#define lcdwBackLight(light) lcdBackLight(workenv, light)

lcdbrightness_t lcdGetBackLight(lcdenv_t* env);
#define lcdwGetBackLight() lcdGetBackLight(workenv)

//void lcdPutRaw(char c);

/**
 * Put raw char to terminal
 * @param c - one character to print
 */
void lcdPutc(lcdenv_t* env, char c);
#define lcdwPutc(c) lcdPutc(workenv, c)

/**
 * Put string to terminal
 * @param ASCIIZ string to print
 */
void lcdPuts(lcdenv_t* env, const char* str);
#define lcdwPuts(str) lcdPuts(workenv, str)

#endif /* _LCD_H_ */

