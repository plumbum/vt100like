#include "kbd.h"

#include <string.h>
#include <stdlib.h>

#include "perif_io.h"

#include "utils.h"

EVENTSOURCE_DECL(kbdKeypress);
EVENTSOURCE_DECL(kbdPressedOn);
EVENTSOURCE_DECL(kbdPressedOff);

static uint8_t kbdbuf[16];
static INPUTQUEUE_DECL(qkbd, kbdbuf, sizeof(kbdbuf), NULL);

static int scan_code;
static char key;

static WORKING_AREA(waKeyboardThread, 128);
static msg_t keyboardThread(void *arg)
{
    (void)arg;

    int _scan_code;
    while(1)
    {
        _scan_code = 0;

        msg_t skey = sdGetTimeout(&SD1, TIME_IMMEDIATE);
        if(skey > 0)
            chIQPutI(&qkbd, skey);

        int i;
        int mask = 1;
        for(i=0; i<4; i++)
        {
            perifSetKbdOut(mask);
            chThdSleepMilliseconds(10);
            _scan_code = (_scan_code << 4) | perifKbdIn();
            mask <<= 1;
        }

        if((scan_code == 0) && (_scan_code != 0))
        {
            int haskey = -1;
            switch(_scan_code)
            {
                case 0x0008: chIQPutI(&qkbd, kbUP); break;
                case 0x0004: chIQPutI(&qkbd, kbDOWN); break;
                case 0x0002: chIQPutI(&qkbd, kbESC); break;
                case 0x0001: chIQPutI(&qkbd, kbENTER); break;
                case 0x1000: chIQPutI(&qkbd, '0'); break;
                case 0x8000: chIQPutI(&qkbd, '1'); break;
                case 0x0400: chIQPutI(&qkbd, '2'); break;
                case 0x0040: chIQPutI(&qkbd, '3'); break;
                case 0x4000: chIQPutI(&qkbd, '4'); break;
                case 0x0200: chIQPutI(&qkbd, '5'); break;
                case 0x0020: chIQPutI(&qkbd, '6'); break;
                case 0x2000: chIQPutI(&qkbd, '7'); break;
                case 0x0100: chIQPutI(&qkbd, '8'); break;
                case 0x0010: chIQPutI(&qkbd, '9'); break;
                case 0x0800:
                    chIQPutI(&qkbd, kbRUN);
                    chEvtBroadcastI(&kbdPressedOn);
                    break;
                case 0x0080:
                    chIQPutI(&qkbd, kbSTOP);
                    chEvtBroadcastI(&kbdPressedOff);
                    break;
                default:
                    haskey = 0;
            }
            if(haskey)
                chEvtBroadcastI(&kbdKeypress);
        }
        scan_code = _scan_code;
    }
    return 0;
}

void kbdInit(void)
{
    scan_code = 0;
    key = 0;
    // Static initialization: chIQInit(&qkbd, kbdbuf, sizeof(kbdbuf), NULL);
    chThdCreateStatic(waKeyboardThread, sizeof(waKeyboardThread), NORMALPRIO-8, keyboardThread, NULL);
}

int kbdHasKey(void)
{
    return chQSpace(&qkbd);
}

int kbdGetChar(void)
{
    return chIQGet(&qkbd);
}

int kbdGetCharImmediate(void)
{
    return chIQGetTimeout(&qkbd, TIME_IMMEDIATE);
}

int kbdGetCharTimeout(systime_t delay)
{
    return chIQGetTimeout(&qkbd, delay);
}

char kbdInputNumeric(lcdenv_t* env, int* value, int min_val, int max_val, onchange_f on_inc_change)
{
    char buf[16];
    char* cp = buf;
    int key;
    int tvalue = *value;
    int max_len, min_len;
    unsigned char row, col;

    max_len = numericLength(max_val);
    min_len = numericLength(min_val);
    if(min_len > max_len) max_len = min_len;

    row = lcdGetRow(env);
    col = lcdGetCol(env);

    if((min_val <= tvalue) && (tvalue <= max_val))
        itoa(tvalue, buf);
    else
        buf[0] = 0;
    lcdPuts(env, buf);

    cp = buf+strlen(buf);

    do
    {
        lcdSetCursor(env, lctBLINK);
        do
        {
            key = kbdGetChar();
            if((key == kbUP) || (key == kbDOWN)) // Escape code
            {
                int chg = 0;
                tvalue = atoi(buf);
                if(key == kbUP) // Arrow UP
                {
                    if(tvalue < max_val) { tvalue++; chg++; }
                }
                else // Arrow DOWN
                {
                    if(tvalue > min_val) { tvalue--; chg++; }
                }
                if(chg)
                {
                    if(on_inc_change)
                        tvalue = on_inc_change(tvalue);
                    itoa(tvalue, buf);
                    lcdGoto(env, row, col);
                    lcdPuts(env, buf);
                    lcdClrEndl(env);
                    cp = buf+strlen(buf);
                }
            }
            else if((key >= '0') && (key <= '9'))
            {
                if((cp-buf) < max_len)
                {
                    *cp++ = key;
                    *cp = 0;
                    lcdPutchar(env, key);
                }
            }
            else
            if((key == kbESC) && (cp > buf))
            {
                cp--;
                *cp = 0;
                lcdPuts(env, "\033D \033D");
                key = 0;
            }
        } while ((key != kbESC) && (key != kbENTER));
        lcdSetCursor(env, lctOFF);
        if(key == kbESC) return key;
        tvalue = atoi(buf);
    } while((tvalue < min_val) || (tvalue > max_val));
    *value = tvalue;
    return key;
}


