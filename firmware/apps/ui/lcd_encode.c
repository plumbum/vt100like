#include "lcd_encode.h"

static const char enctable90[] = {
    0x00, // x90
    0x00,
    0x00,
    0xca,
    0xcb,
    0x00,
    0x00,
    0x00,

    0x00,
    0x00,
    0x00,
    0x00,
    0xd9, // Arrow up
    0xda, // Arrow down
    0x00,
    0x00,

    0x00, // xA0
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,

    0xa2,
    0x00,
    0x00,
    0xc8,
    0x00,
    0x00,
    0x00,
    0x00,

    0x00, // xB0
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,

    0xb5,
    0xcc,
    0x00,
    0xc9,
    0x00,
    0x00,
    0x00,
    0x00,

    0x41, // А 
    0xa0, // Б 
    0x42, // В 
    0xa1, // Г 
    0xe0, // Д 
    0x45, // Е 
    0xa3, // Ж 
    0x33, // З 
    0xa5, // И 
    0xa6, // Й 
    0x4b, // К 
    0xa7, // Л 
    0x4d, // М 
    0x48, // Н 
    0x4f, // О 
    0xa8, // П 
    0x50, // Р 
    0x43, // С 
    0x54, // Т 
    0xa9, // У 
    0xaa, // Ф 
    0x58, // Х 
    0xe1, // Ц 
    0xab, // Ч 
    0xac, // Ш 
    0xe2, // Щ 
    0xad, // Ъ 
    0xae, // Ы 
    0x62, // Ь 
    0xaf, // Э 
    0xb0, // Ю 
    0xb1, // Я 

    0x61, // а 
    0xb2, // б 
    0xb3, // в 
    0xb4, // г 
    0xe3, // д 
    0x65, // е 
    0xb6, // ж 
    0xb7, // з 
    0xb8, // и 
    0xb9, // й 
    0xba, // к 
    0xbb, // л 
    0xbc, // м 
    0xbd, // н 
    0x6f, // о 
    0xbe, // п 
    0x70, // р 
    0x63, // с 
    0xbf, // т 
    0x79, // у 
    0xe4, // ф 
    0x78, // х 
    0xe5, // ц 
    0xc0, // ч 
    0xc1, // ш 
    0xe6, // щ 
    0xc2, // ъ 
    0xc3, // ы 
    0xc4, // ь 
    0xc5, // э 
    0xc6, // ю 
    0xc7, // я 
};


char encodeLcd(char c)
{
    if(c >= 0x90)
        return enctable90[c-0x90];
    else
        return c;
}

