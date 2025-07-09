#ifndef _LIBRARY_LIST_
#define _LIBRARY_LIST_
#include "drawpanel.h"

#include <dirent.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
typedef struct
{
    char *filename;
    long int size;
    char *modify_time;
    int is_dir;
} file;

typedef struct
{
    int active;
    char cwd[PATH_MAX];
    int current_now;
    int file_count;
    file *filelist;
    WINDOW *win;
    WINDOW *name_win;
    WINDOW *size_win;
    WINDOW *time_win;
} Panel;

extern Panel left, right;
extern Panel *active_panel;

void listdirect(int panel_height, Panel *win);
#endif