#include "initpanels.h"
#include <stdlib.h>
void init_panels(int panel_width, int panel_height)
{
    left.active = 1;
    left.current_now = 0;
    left.file_count = 0;
    getcwd(left.cwd, sizeof(left.cwd));
    left.win = newwin(panel_height, panel_width, 1, 0);
    left.name_win = newwin(panel_height - 2, panel_width * 0.6, 2, 1);
    left.size_win = newwin(panel_height - 2, panel_width * 0.4, 2, panel_width * 0.4);
    left.time_win = newwin(panel_height - 2, panel_width * 0.4, 2, panel_width * 0.6);
    left.filelist = malloc(sizeof(file) * 20);
    if (left.filelist == NULL)
    {

        printw("Память не выделилась");
        refresh();
        endwin();
        exit(1);
    }
    right.active = 0;
    right.current_now = 0;
    right.file_count = 0;
    getcwd(right.cwd, sizeof(right.cwd));
    right.win = newwin(panel_height, panel_width, 1, panel_width);
    right.name_win = newwin(panel_height - 2, panel_width * 0.7, 2, panel_width + 1);
    right.size_win = newwin(panel_height - 2, panel_width * 0.5, 2, panel_width * 1.4);
    right.time_win = newwin(panel_height - 2, panel_width * 0.4 - 2, 2, panel_width * 1.6);
    right.filelist = malloc(sizeof(file) * 20);
    if (right.filelist == NULL)
    {

        printw("Память не выделилась");
        refresh();
        free(left.filelist);
        endwin();
        exit(1);
    }
    active_panel = &left;
}