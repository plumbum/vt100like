#include "menu.h"

#include <ch.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lcd.h"

static lcdenv_t* workenv;

void menuInit(lcdenv_t* env)
{
    workenv = env;
}

void menuShow(struct menu_item_t* menu)
{
    lcdHwInit(); // TODO dirty hack
    lcdwClr();
    while(1)
    {
        if(menu->text)
        {
            if(menu->col >= 0)
            {
                if(menu->row == -1)
                    { lcdwPuts(EOLN); }
                else
                    lcdwGoto(menu->row, menu->col);
            }
            lcdwPuts(menu->text);
        }

        if(menu->preprint)
            (void)(struct menu_item_t*)(menu->preprint((void*)menu));

        if(menu->last_item != 0)
            break;
        menu++;
    }
}

struct menu_item_t* menuAction(struct menu_item_t* menu)
{
    char key;
    struct menu_item_t* next_menu;
    struct menu_item_t* cur_item;

    while(1)
    {
        key = kbdGetCharImmediate();
        if(key != 0)
        {
            cur_item = menu;
            while(1)
            {
                char menu_key = cur_item->key;
                if((menu_key == 0) && (cur_item->text != NULL)) // If key not defined get first char from title
                    menu_key = cur_item->text[0];
                if(menu_key == key)
                {
                    next_menu = 0;
                    if(cur_item->action)
                        next_menu = (struct menu_item_t*)(cur_item->action((void*)cur_item));
                    if(next_menu == 0)
                        next_menu = cur_item->action_menu;
                    return next_menu;
                }
                if(cur_item->last_item != 0)
                    break;
                cur_item++;
            }
        }
        chThdSleepMilliseconds(20);
    }
}


int menuControl(struct menu_item_t* init_menu)
{
    struct menu_item_t* menu = init_menu;
    while(1)
    {
        menuShow(menu);
        menu = menuAction(menu);
        if(menu == 0)
            menu = init_menu;
    }
    return 0;
}



