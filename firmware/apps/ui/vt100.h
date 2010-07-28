#ifndef _VT100_H_
#define _VT100_H_

#include "lcd.h"

typedef enum {
    vtsIdle = 0,
    vtsEsc,
    vtsBracket,
    vtsBracketA,
    vtsBracketB,
    vtsQuest,
    vtsHash,
} vtState_t;

#define VT_MAX_PARAM 3

typedef struct {
    lcdenv_t* lcdenv;       /* Related LCD environment */
    vtState_t state;        /* finite-state machine */
    int8_t params;          /* Parametrs count */
    uint16_t param[VT_MAX_PARAM];      /* Parametrs storage */

    /* Saved position & attributes */
    lcdpos_t saveRow;
    lcdpos_t saveCol;
} vtEnv_t;


/**
 * Init terminal and state machine
 */
void vtInit(vtEnv_t* vtenv, lcdenv_t* lcdenv);

/**
 * Reset terminal and state machine
 */
void vtReset(vtEnv_t* vtenv);

/**
 * Put char to VT100 terminal
 */
void vtPutc(vtEnv_t* vtenv, const char c);

/**
 * Put string to VT100 terminal
 */
void vtPuts(vtEnv_t* vtenv, const char* str);

#endif /* _VT100_H_ */

