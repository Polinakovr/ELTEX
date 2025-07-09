#ifndef _LIBRARY_INIT_
#define _LIBRARY_INIT_
#include "drawpanel.h"
#include "listdirectory.h"

#include <dirent.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

void init_panels(int panel_width, int panel_height);
#endif