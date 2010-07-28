
static char _hexChar(int v)
{
    v &= 0x0F;
    if(v<10)
        return '0'+v;
    else
        return 'A'+v-10;
}

void lcdPutHex(lcdenv_t* workenv, uint32_t val, int digits)
{
    if(digits >= 8) lcdPutc(workenv, _hexChar(val >> 28));
    if(digits >= 7) lcdPutc(workenv, _hexChar(val >> 24));
    if(digits >= 6) lcdPutc(workenv, _hexChar(val >> 20));
    if(digits >= 5) lcdPutc(workenv, _hexChar(val >> 16));
    if(digits >= 4) lcdPutc(workenv, _hexChar(val >> 12));
    if(digits >= 3) lcdPutc(workenv, _hexChar(val >> 8));
    if(digits >= 2) lcdPutc(workenv, _hexChar(val >> 4));
    lcdPutc(workenv, _hexChar(val >> 0));
}

void lcdPutui(lcdenv_t* workenv, uint32_t val)
{
    uint32_t ipow;
    uint32_t power;
    char atmp;
    char prefix_flag = 0;
    for(ipow=0; ipow<(sizeof(power10)/sizeof(power10[0])); ipow++)
    {
        atmp = '0';
        power = power10[ipow];
        while(val >= power)
        {
            atmp++;
            val -= power;
        }
        if(atmp > '0')
            prefix_flag = -1;
        if(prefix_flag)
            lcdPutc(workenv, atmp);
    }
    lcdPutc(workenv, val+'0');
}

void lcdPuti(lcdenv_t* workenv, int32_t val)
{
    if(val<0)
    {
        lcdPutc(workenv, '-');
        lcdPutui(workenv, -val);
    }
    else
        lcdPutui(workenv, val);
}

