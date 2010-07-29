#include <ch.h>
#include <hal.h>

#include "lcd.h"
#include "vt100.h"
#include "utils.h"

static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

    (void)arg;
    while (1)
    {
        /*
        GPIOB->BSRR = 1;
        chThdSleepMilliseconds(500);
        GPIOB->BRR = 1;
        */
        chThdSleepMilliseconds(50);
    }
    return 0;
}

lcdenv_t env;
lcdenv_t* workenv;
vtEnv_t vt;

int main(int argc, char **argv) {

    (void)argc;
    (void)argv;

    sdStart(&SD1, NULL);

    /*
     * Creates the blinker thread.
     */
    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

    workenv = &env;
    lcdInit(&env);
    lcdwBackLight(100);

    vtInit(&vt, &env);
    vtPuts(&vt, "Hello world");

    vtPuts(&vt, "\033cNew age\033[2B\033[5CLine it\b\r\nWow!");

    int cnt = 0;
    char szBuf[16];
    while (1)
    {
        vtPuts(&vt, "\033[2;4fCnt: ");
        itoa(chTimeNow(), szBuf);
        vtPuts(&vt, szBuf);
        vtPuts(&vt, "\033[K");
        cnt++;
        chThdSleepMilliseconds(1);
    }
    return 0;
}
