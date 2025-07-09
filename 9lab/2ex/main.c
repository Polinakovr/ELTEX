#include "drawpanel.h"
#include "initpanels.h"
#include "listdirectory.h"

#include <dirent.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

Panel left, right;
Panel *active_panel;

void free_list(Panel p)
{
    for (int i = 0; i < p.file_count; i++)
    {
        free(p.filelist[i].filename);
        free(p.filelist[i].modify_time);
    }
    free(p.filelist);
}

void switchactive()
{
    if (active_panel == &left)
    {
        left.active = 0;
        right.active = 1;
        active_panel = &right;
    }
    else
    {
        right.active = 0;
        left.active = 1;
        active_panel = &left;
    }
}

int main()
{
    initscr();
    keypad(stdscr, TRUE);
    clear();
    cbreak();
    noecho();
    curs_set(0);

    if (!has_colors())
    {
        printw("Терминал не поддерживает цвета");
        refresh();
        endwin();
        exit(1);
    }

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_CYAN);
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int panel_height = max_y - 2;
    int panel_width = max_x / 2;

    WINDOW *win_top = newwin(1, max_x, 0, 0);

    init_panels(panel_width, panel_height);

    char left_cwd[PATH_MAX], right_cwd[PATH_MAX];
    getcwd(left_cwd, sizeof(left_cwd));
    getcwd(right_cwd, sizeof(right_cwd));
    strcpy(left.cwd, left_cwd);
    strcpy(right.cwd, right_cwd);

    while (1)
    {
        draw_panel(win_top, 1, "LEFT  FILE  COMMAND  OPTIONS  RIGHT");
        draw_panel(left.win, 0, left.cwd);
        draw_panel(right.win, 0, right.cwd);

        chdir(left_cwd);
        listdirect(panel_height, &left);
        chdir(right_cwd);
        listdirect(panel_height, &right);

        if (active_panel == &left)
        {
            chdir(left_cwd);
        }
        else
        {
            chdir(right_cwd);
        }

        int c = getch();
        if (c == 'q')
            break;

        switch (c)
        {
            case KEY_UP:
                if (active_panel->current_now > 0)
                {
                    (active_panel->current_now)--;
                }
                break;

            case KEY_DOWN:
                if (active_panel->current_now < (active_panel->file_count) - 1)
                {
                    (active_panel->current_now)++;
                }
                break;

            case '\n':
                if (active_panel->filelist[active_panel->current_now].is_dir)
                {
                    char new_path[PATH_MAX];
                    if (active_panel == &left)
                    {
                        snprintf(new_path, sizeof(new_path), "%s/%s", left_cwd,
                                 active_panel->filelist[active_panel->current_now].filename);
                        if (chdir(new_path) == 0)
                        {
                            getcwd(left_cwd, sizeof(left_cwd));
                            strcpy(left.cwd, left_cwd);
                            active_panel->current_now = 0;
                        }
                    }
                    else
                    {
                        snprintf(new_path, sizeof(new_path), "%s/%s", right_cwd,
                                 active_panel->filelist[active_panel->current_now].filename);
                        if (chdir(new_path) == 0)
                        {
                            getcwd(right_cwd, sizeof(right_cwd));
                            strcpy(right.cwd, right_cwd);
                            active_panel->current_now = 0;
                        }
                    }
                }
                break;

            case '\t': switchactive(); break;
        }
    }

    free_list(left);
    free_list(right);
    delwin(left.name_win);
    delwin(right.name_win);
    delwin(left.size_win);
    delwin(right.size_win);
    delwin(right.win);
    delwin(left.win);
    delwin(win_top);
    endwin();
    return 0;
}