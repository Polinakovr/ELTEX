#ifndef _LIBRARY_INIT_
#define _LIBRARY_INIT_
#include <ncurses.h>
#include "drawpanel.h"
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "listdirectory.h"

void init_panels(int panel_width, int panel_height);
#endif