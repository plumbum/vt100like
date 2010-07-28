#include "vt100.h"

void vtInit(vtEnv_t* vtenv, lcdenv_t* lcdenv)
{
    vtenv->lcdenv = lcdenv;
    vtReset(vtenv);
}

void vtReset(vtEnv_t* vtenv)
{
    vtenv->state = vtsIdle;
    vtenv->params = 0;
    vtenv->saveRow = 1;
    vtenv->saveCol = 1;
    lcdClr(vtenv->lcdenv);
}

void vtPutc(vtEnv_t* vtenv, const char c)
{
    lcdenv_t* workenv = vtenv->lcdenv;
    switch(vtenv->state)
    {
        case vtsEsc:
            switch(c)
            {
                case 'c': // Reset terminal
                    vtReset(vtenv);
                    vtenv->state = vtsIdle;
                    break;
                case 'D': // Scroll down
                    // TODO
                    vtenv->state = vtsIdle;
                    break;
                case 'M': // Scroll up
                    // TODO
                    vtenv->state = vtsIdle;
                    break;
                case 'E':
                    lcdwHomeLine();
                    lcdwLineFeed();
                    vtenv->state = vtsIdle;
                    break;
                case '7': // Save cursor & attrs
                    vtenv->saveRow = lcdwGetRow()+1;
                    vtenv->saveCol = lcdwGetCol()+1;
                    // TODO save attrs
                    vtenv->state = vtsIdle;
                    break;
                case '8': // Restore cursor & attrs
                    lcdwGoto(vtenv->saveRow, vtenv->saveCol);
                    vtenv->state = vtsIdle;
                    break;
                case '[': // Command prefix
                    vtenv->state = vtsBracket;
                    for(uint8_t i=0; i<VT_MAX_PARAM; i++)
                        vtenv->param[i] = 0;
                    vtenv->params = 0;
                    break;
                case '(':
                    vtenv->state = vtsBracketA;
                    break;
                case ')':
                    vtenv->state = vtsBracketB;
                    break;
                case '#':
                    vtenv->state = vtsHash;
                    break;
                default:
                    vtenv->state = vtsIdle;
            }
            break;
        case vtsHash:
        case vtsBracketA:
        case vtsBracketB: // Dummy functions
            vtenv->state = vtsIdle;
            break;
        case vtsBracket: // Process commands after bracket
        case vtsQuest: // <ESC> [ ? Pn commands
            switch(c)
            {
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                    // TODO so dirty
                    vtenv->param[vtenv->params] = (vtenv->param[vtenv->params]*10) + (c-'0');
                    break;
                case ';':
                    vtenv->params++;
                    break;
                case '?':
                    vtenv->state = vtsQuest;
                    break;
                case 'h':
                case 'l':
                    switch(vtenv->param[0])
                    {
                        case 50: case 25:
                            if(c == 'h')
                                lcdwSetCursor(lctBLINK);
                            else
                                lcdwSetCursor(lctOFF);
                            break;
                    }
                    vtenv->state = vtsIdle;
                    break;
                case 'A':
                    lcdwUp(vtenv->param[0]?vtenv->param[0]:1);
                    vtenv->state = vtsIdle;
                    break;
                case 'B':
                    lcdwDown(vtenv->param[0]?vtenv->param[0]:1);
                    vtenv->state = vtsIdle;
                    break;
                case 'C':
                    lcdwRight(vtenv->param[0]?vtenv->param[0]:1);
                    vtenv->state = vtsIdle;
                    break;
                case 'D':
                    lcdwLeft(vtenv->param[0]?vtenv->param[0]:1);
                    vtenv->state = vtsIdle;
                    break;
                case 'H': case 'f': // Set cursor position
                    lcdwGoto(
                            (vtenv->param[0]?vtenv->param[0]:1)-1,
                            (vtenv->param[1]?vtenv->param[1]:1)-1
                    );
                    vtenv->state = vtsIdle;
                    break;

                case 'J': // Erase lines
                    switch(vtenv->param[0])
                    {
                        case 0: // Erases lines from current to end
                            for(lcdpos_t i=lcdwGetRow(); i<lcdGetRows(); ++i)
                                lcdwClrLine(i);
                            break;
                        case 1: // Erases line from current to home
                            for(lcdpos_t i=0; i<=lcdwGetRow(); ++i)
                                lcdwClrLine(i);
                            break;
                        case 2:
                            lcdwClr();
                            break;
                    }
                    vtenv->state = vtsIdle;
                    break;

                case 'K':
                    switch(vtenv->param[0])
                    {
                        case 0: // Erase to end line
                            lcdwClrEndl();
                            break;
                        case 1: // Erase to start line
                            lcdwClrCols(lcdwGetRow(), 0, lcdwGetCol());
                            break;
                        case 2: // Erase line
                            lcdwClrLine(lcdwGetRow());
                            break;
                    }
                    vtenv->state = vtsIdle;
                    break;

                default:
                    vtenv->state = vtsIdle;
            }
            break;
        default: // vtsIdle
            switch(c)
            {
                case 0x1B: // ESC code
                    vtenv->state = vtsEsc;
                    break;
                case '\n': // LF 10
                    lcdwLineFeed();
                    break;
                case '\r': // CR 13 Carridge return
                    lcdwSetCol(0);
                    break;
                case '\a': // BELL 7
                    // TODO Bell
                    break;
                case '\b': // BS 8
                    lcdwBackspace();
                    break;
                case '\t': // HT 9
                    lcdwTab();
                    break;
                case '\v': // VT 11
                    // TODO VerticalTab
                    break;
                case '\f': // FF 12
                    // TODO FormFeed
                    break;
                default:
                    if(c >= ' ')
                        lcdwPutc(c);
            }
    }
}


void vtPuts(vtEnv_t* vtenv, const char* str)
{
    char c;
    while((c = *str++) != 0)
        vtPutc(vtenv, c);
}

