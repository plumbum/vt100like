#ifndef _KBD_H_
#define _KBD_H_

#include <ch.h>
#include <hal.h>
#include "lcd.h"


// Ctrl-A : SOH Start of heading
#define kbRUN       0x01
// Ctrl-C : ETX End of text
#define kbSTOP      0x03

// Ctrl-K : VT
#define kbUP        0x0B
// Ctrl-J : LF 
#define kbDOWN      0x0A

// Ctrl-M : CR
#define kbENTER     0x0D
// Ctrl-H : BS
#define kbESC       0x08


typedef int(*onchange_f)(int);

extern EventSource kbdKeypress;
extern EventSource kbdPressedOn;
extern EventSource kbdPressedOff;

void kbdInit(void);

/**
 * Reset keyboard buffer
 */
void kbdReset(void);

/**
 * Get pressed key
 */
int kbdGetChar(void);

/**
 * Try to immediate get key
 * If no key ready, then return Q_TIMEOUT
 */
int kbdGetCharImmediate(void);

int kbdGetCharTimeout(systime_t delay);

int kbdHasKey(void);


// uint16_t kbdGetScanCode(void);

char kbdInputNumeric(lcdenv_t* env, int* value, int min_val, int max_val, onchange_f on_inc_change);

#endif /* _KBD_H_ */

