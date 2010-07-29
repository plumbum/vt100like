#include "ch.h"
#include "hal.h"

/*
 * Red LEDs blinker thread, times are in milliseconds.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

    (void)arg;
    while (1)
    {
        GPIOD->BSRR = (1<<2);
        chThdSleepMilliseconds(500);
        GPIOD->BRR = (1<<2);
        chThdSleepMilliseconds(500);
    }
    return 0;
}

static void _lcdDelay(int i)
{
    while(i--)
        asm volatile (" nop ");
}

static uint8_t _lcdRead(void)
{
    GPIOC->CRL = 0x88888888;

    GPIOC->CRL = 0x33333333;
}

void _lcdBusy(void)
{
    uint8_t r;
    GPIOC->CRL = 0x88888888;
    GPIOC->BSRR = (1<<8) | (1<<9); // RS & R/W write
    do
    {
        GPIOC->BSRR = (1<<10); // E enable mark
        _lcdDelay(5);
        r = GPIOC->IDR & 0xFF;
        GPIOC->BRR = (1<<10); // E enable release
        _lcdDelay(5);
    } while(r & 0x80);
    
    GPIOC->CRL = 0x22222222;
}

static void _lcdWrite(uint8_t d)
{
    GPIOC->BRR = 0xFF;
    GPIOC->BRR = (1<<9); // R/W write
    GPIOC->BSRR = d;
    _lcdDelay(5);
    GPIOC->BSRR = (1<<10); // E enable mark
    _lcdDelay(5);
    GPIOC->BRR = (1<<10); // E enable release
    _lcdDelay(5);
}

void _lcdWriteReg(uint8_t reg, uint8_t data)
{
    //chThdSleepMilliseconds(1);
    _lcdBusy();
    GPIOC->BSRR = (1<<8); // RS
    _lcdWrite(reg);
    GPIOC->BRR = (1<<8); // RS
    _lcdWrite(data);
}

void lcdInit(void)
{
    GPIOC->BRR = (1<<11); // Reset
    chThdSleepMilliseconds(50);
    GPIOC->BSRR = (1<<11);
    chThdSleepMilliseconds(50);
    _lcdWriteReg(0, 0x32); // Disp on, master, graphic
    //_lcdWriteReg(0, 0x12); // master, graphic
    _lcdWriteReg(1, 8-1); // Horizontal character pitch
    _lcdWriteReg(2, 20-1); // bytes in horizontal direction
    _lcdWriteReg(3, 80-1); // Display duty
    //_lcdWriteReg(4, 0x07); // Cursor position
    _lcdWriteReg(8, 0); // Start address LB
    _lcdWriteReg(9, 0); // Start address UB

    _lcdWriteReg(10, 0); // Start address LB
    _lcdWriteReg(11, 0); // Start address UB

    for(int i=0; i<160*80/8; i++)
    {
        _lcdWriteReg(0x0C, i);
    }

    _lcdWriteReg(10,   0); _lcdWriteReg(11, 0); _lcdWriteReg(0x0C, 0x18); _lcdWriteReg(0x0C, 0xFC);
    _lcdWriteReg(10,  20); _lcdWriteReg(11, 0); _lcdWriteReg(0x0C, 0x3C); _lcdWriteReg(0x0C, 0xFE);
    _lcdWriteReg(10,  40); _lcdWriteReg(11, 0); _lcdWriteReg(0x0C, 0x66); _lcdWriteReg(0x0C, 0xC3);
    _lcdWriteReg(10,  60); _lcdWriteReg(11, 0); _lcdWriteReg(0x0C, 0xC3); _lcdWriteReg(0x0C, 0xFE);
    _lcdWriteReg(10,  80); _lcdWriteReg(11, 0); _lcdWriteReg(0x0C, 0xC3); _lcdWriteReg(0x0C, 0xFE);
    _lcdWriteReg(10, 100); _lcdWriteReg(11, 0); _lcdWriteReg(0x0C, 0xFF); _lcdWriteReg(0x0C, 0xC3);
    _lcdWriteReg(10, 120); _lcdWriteReg(11, 0); _lcdWriteReg(0x0C, 0xC3); _lcdWriteReg(0x0C, 0xFE);
    _lcdWriteReg(10, 140); _lcdWriteReg(11, 0); _lcdWriteReg(0x0C, 0xC3); _lcdWriteReg(0x0C, 0xFC);
}

int main(int argc, char **argv) {

    (void)argc;
    (void)argv;

    /*
     * Activates the serial driver 2 using the driver default configuration.
     */
    sdStart(&SD1, NULL);

    lcdInit();
    /*
     * Creates the blinker thread.
     */
    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);


    /*
     * Normal main() thread activity, in this demo it does nothing except
     * sleeping in a loop and check the button state.
     */
    int pos = 0;
    int dir = 20;
    while (1)
    {
        _lcdWriteReg(8, pos & 0xFF); // Start address LB
        _lcdWriteReg(9, pos>>8); // Start address UB

        pos += dir;
        if(pos >= 2000) dir = -dir;
        if(pos <= 0) dir = -dir;
        chThdSleepMilliseconds(20);
    }
    return 0;
}
