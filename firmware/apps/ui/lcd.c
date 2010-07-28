//******************************************************************************
//*
//*     FULLNAME:  LCD hi lever driver
//*
//*     FILENAME:  lcd.c
//*
//*     PROCESSOR: abstract
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

#include "lcd.h"
#include "lcdhw.h"

#include <string.h>

/**
 * Space char
 */
#define SPACE 0x20


/**
 * @block Hi level function with environments support
 */
lcdenv_t* workenv;
static lcdenv_t* dispenv;

void lcdHwInit(void)
{
    _lcd_init();
}

void lcdInit(lcdenv_t* env)
{
    _lcd_init();
    dispenv = 0;
    lcdInitEnv(env);
    lcdSetDispEnv(env);
}

void lcdSetCursor(lcdenv_t* workenv, lcdcursor_t cursor)
{
    if(workenv == dispenv)
        _lcdSetCursor(cursor);
    workenv->cursor = cursor;
}

void lcdGoto(lcdenv_t* workenv, lcdpos_t row, lcdpos_t col)
{
    if(workenv == dispenv)
        _lcdGoto(row, col);
    workenv->row = row;
    workenv->col = col;
}

void lcdHome(lcdenv_t* workenv)
{
    if(workenv == dispenv)
        _lcdHome();
    workenv->row = 0;
    workenv->col = 0;
}

void lcdClr(lcdenv_t* workenv)
{
    // TODO memset
    int i;
    char* p = (char*)workenv->dbuf;
    for(i=0; i<lcdGetRows()*lcdGetCols(); i++)
        *p++ = SPACE;
    if(workenv == dispenv)
        _lcdClr();
    workenv->row = 0;
    workenv->col = 0;
}

void lcdScreenRefresh(lcdpos_t row1, lcdpos_t row2)
{
    lcdpos_t row;
    lcdpos_t col;
    char* p;
    _lcdSetCursor(lctOFF);
    for(row=row1; row<=row2; row++)
    {
        _lcdGoto(row, 0);
        p = dispenv->dbuf[row];
        for(col=0; col<lcdGetCols(); col++)
            lcdData(*p++);
    }
    _lcdGoto(dispenv->row, dispenv->col);
    _lcdSetCursor(dispenv->cursor);
}

void lcdScreenScrollUp(lcdenv_t* workenv, lcdpos_t row1, lcdpos_t row2)
{
    memcpy(workenv->dbuf[row1], workenv->dbuf[row1+1], lcdGetCols()*(row2-row1));
    memset(workenv->dbuf[row2], SPACE, lcdGetCols());
    if(workenv == dispenv)
        lcdScreenRefresh(row1, row2);
}

void lcdHomeLine(lcdenv_t* workenv)
{
    lcdGoto(workenv, workenv->row, 0);
}

void lcdEndLine(lcdenv_t* workenv)
{
    lcdGoto(workenv, workenv->row, lcdGetCols()-1);
}

void lcdClrLine(lcdenv_t* workenv, lcdpos_t row)
{
    // TODO memset
    int i;
    char* p = workenv->dbuf[row];
    if(workenv == dispenv)
    {
        _lcdGoto(row, 0);
        for(i=0; i<lcdGetCols(); i++)
        {
            lcdData(SPACE);
            *p++ = SPACE;
        }
        _lcdGoto(row, 0);
    }
    else
        for(i=0; i<lcdGetCols(); i++)
            *p++ = SPACE;
}

void lcdClrCols(lcdenv_t* workenv, lcdpos_t row, lcdpos_t colfrom, lcdpos_t colto)
{
    // TODO memset
    int i;
    char* p = &(workenv->dbuf[row][colfrom]);
    if(workenv == dispenv)
    {
        _lcdGoto(row, colfrom);
        for(i=colfrom; i<=colto; i++)
        {
            lcdData(SPACE);
            *p++ = SPACE;
        }
        _lcdGoto(row, colfrom);
    }
    else
        for(i=colfrom; i<=colto; i++)
            *p++ = SPACE;
}

void lcdBackLight(lcdenv_t* workenv, lcdbrightness_t light)
{
    workenv->light = light;
    if(workenv == dispenv)
        _lcdBackLight(light);
}

lcdbrightness_t lcdGetBackLight(lcdenv_t* workenv)
{
    return workenv->light;
}

void lcdInitEnv(lcdenv_t* env)
{
    env->tabsize = 4;
    env->inverse = 0;
    env->bold = 0;
    env->wrap = 0;
    env->scroll = 0;
    env->needrepos = 0;
    env->frame_row1 = 0;
    env->frame_row2 = lcdGetRows()-1;
#if (LCD_COLOR == 1)
    // TODO !!!
    env->fg = 0;
    env->bg = 0;
#endif /* LCD_COLOR */
    lcdClr(env);
    lcdSetCursor(env, lctOFF);
    lcdBackLight(env, ldtOFF);
}

/*
void lcdSetWorkEnv(lcdenv_t* env)
{
    workenv = env;
}

lcdenv_t* lcdGetWorkEnv(void)
{
    return workenv;
}
*/

void lcdSetDispEnv(lcdenv_t* env)
{
    if(dispenv != env)
    {
        dispenv = env;
        lcdScreenRefresh(0, lcdGetRows()-1);
        _lcdGoto(dispenv->row, dispenv->col);
        _lcdSetCursor(dispenv->cursor);
        _lcdBackLight(dispenv->light);
    }
}

lcdenv_t* lcdGetDispEnv(void)
{
    return dispenv;
}

lcdpos_t lcdGetRow(lcdenv_t* workenv)
{
    return workenv->row;
}

lcdpos_t lcdGetCol(lcdenv_t* workenv)
{
    return workenv->col;
}

void lcdTab(lcdenv_t* workenv)
{
    // TODO work only if tabsize is power of 2
    lcdpos_t end = (workenv->col & ~(workenv->tabsize-1)) + workenv->tabsize;
    lcdpos_t i;
    for(i = workenv->col; i < end; i++)
        lcdPutc(workenv, SPACE);
}

void lcdBackspace(lcdenv_t* workenv)
{
    if(workenv->col>0)
        workenv->col--;
    else
    {
        if(workenv->row>0)
        {
            workenv->col = lcdGetCols()-1;
            workenv->row--;
        }
    }
    workenv->dbuf[workenv->row][workenv->col] = SPACE;
    if(workenv == dispenv)
    {
        _lcdGoto(workenv->row, workenv->col);
        lcdData(SPACE);
        workenv->needrepos = 0;
        _lcdGoto(workenv->row, workenv->col);
    }
}

void lcdLeft(lcdenv_t* workenv, lcdpos_t size)
{
    int i;
    i = workenv->col - size;
    if(i<0)
        workenv->col = 0;
    else
        workenv->col = i;
    if(workenv == dispenv)
        _lcdGoto(dispenv->row, dispenv->col);
}

void lcdRight(lcdenv_t* workenv, lcdpos_t size)
{
    int i;
    i = workenv->col + size;
    if(i>=lcdGetCols())
        workenv->col = lcdGetCols()-1;
    else
        workenv->col = i;
    if(workenv == dispenv)
        _lcdGoto(dispenv->row, dispenv->col);
}

void lcdUp(lcdenv_t* workenv, lcdpos_t size)
{
    int i;
    i = workenv->row - size;
    if(i < workenv->frame_row1) // TODO scroll down
        workenv->row = workenv->frame_row1;
    else
        workenv->row = i;
    if(workenv == dispenv)
        _lcdGoto(dispenv->row, dispenv->col);
}

void lcdDown(lcdenv_t* workenv, lcdpos_t size)
{
    int i;
    i = workenv->row + size;
    if(i > workenv->frame_row2) // TODO scroll up
        workenv->row = workenv->frame_row2;
    else
        workenv->row = i;
    if(workenv == dispenv)
        _lcdGoto(dispenv->row, dispenv->col);
}

void lcdLineFeed(lcdenv_t* workenv)
{
    workenv->row++;
    if(workenv == dispenv)
        if(workenv->row <= workenv->frame_row2)
            _lcdGoto(dispenv->row, dispenv->col);
}

void lcdSetCol(lcdenv_t* workenv, lcdpos_t col)
{
    workenv->col = col;
    if(workenv == dispenv)
        _lcdGoto(dispenv->row, col);
}

void lcdPutc(lcdenv_t* workenv, char c)
{
    if(workenv->row > workenv->frame_row2)
    {
        workenv->row = workenv->frame_row2;
        lcdScreenScrollUp(workenv, workenv->frame_row1, workenv->frame_row2);
    }
    if(workenv->col >= lcdGetCols())
    {
        if(workenv->wrap)
        {
            if(workenv->row < workenv->frame_row2)
            {
                workenv->row++;
            }
            else
                if(workenv->scroll)
                    lcdScreenScrollUp(workenv, workenv->frame_row1, workenv->frame_row2);
            lcdGoto(workenv, workenv->row, 0);
        }
        else /* No line wrap - no print */
            return; 
    }
    if(workenv->dbuf[workenv->row][workenv->col] != c)
    {
        /* Когда в нужной нам позиции уже напечатан точно такой же
         * символ, мы не перепечатываем его заново.
         * Но при этом нам необходимо спозиционироваться при печати
         * следующего символа
         */
        if(workenv == dispenv)
        {
            if(workenv->needrepos)
            {
                workenv->needrepos = 0;
                _lcdGoto(workenv->row, workenv->col);
            }
            lcdEncData(c);
        }
        workenv->dbuf[workenv->row][workenv->col] = c;
    }
    else
        workenv->needrepos = -1;
    workenv->col++;
}

void lcdPuts(lcdenv_t* workenv, const char* str)
{
    char c;
    while((c = *str++) != 0)
        lcdPutc(workenv, c);
}

