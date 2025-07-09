#include "drawpanel.h"
extern int current_now;
extern int file_count;
void draw_panel(WINDOW *win, int is_top, const char *tittle)
{
    werase(win);
    if (is_top)
    {
        wbkgd(win, COLOR_PAIR(2));
    }
    else
    {
        wbkgd(win, COLOR_PAIR(1));
        box(win, 0, 0);
    }
    mvwprintw(win, 0, 2, "%s", tittle);

    wrefresh(win);
}