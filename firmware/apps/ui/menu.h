#ifndef _MENU_H_
#define _MENU_H_

#include <ch.h>

#include "actions.h"
#include "kbd.h"

#define NOACT (action_f)(0)
#define _T(var) (struct menu_item_t*)(var)

// #define kbESC 0x1B
// #define kbENTER 0x0D

typedef int(* action_f)(void*);

struct menu_item_t {
    const char* text; // Menu text
    char key; // Action key
    signed char row; // -1 - CRLF
    signed char col; // -1 - no prefix
    char last_item:1;
    action_f preprint; // On print function
    action_f action; // On key action function
    struct menu_item_t* action_menu; // Action menu list
};

void menuInit(lcdenv_t* env);

int menuControl(struct menu_item_t* init_menu);

#endif /* _MENU_H_ */

