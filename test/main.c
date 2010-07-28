#include <stdio.h>
#include <ncurses.h>

int main(int argc, char** argv)
{	
    (void)argc;
    (void)argv;

    WINDOW* mwin;

	initscr();			/* Start curses mode 		  */
    cbreak();

	printw("Hello World !!!");	/* Print Hello World		  */
	refresh();			/* Print it on to the real screen */

    mwin = newwin(6, 22, 3, 5);
    //wborder(mwin, '|', '|', '-', '-', '+', '+', '+', '+');
    box(mwin, 0, 0);
    mvwprintw(mwin, 1, 1, "Window\n");
    int i;
    for(i=0; i<10; i++)
    {
        wprintw(mwin, "Line %d\n", i);
        wrefresh(mwin);
    }

	getch();			/* Wait for user input */
	endwin();			/* End curses mode		  */

	return 0;
}
