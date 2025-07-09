#include "listdirectory.h"

void listdirect(int panel_height, Panel *win)
{
    DIR *d;
    struct dirent *dir;
    struct stat st;
    d = opendir(".");
    win->file_count = 0;
    werase(win->name_win);
    werase(win->size_win);
    werase(win->time_win);
    box(win->name_win, 0, 0);
    box(win->size_win, 0, 0);
    box(win->time_win, 0, 0);
    draw_panel(win->name_win, 0, "NAME");
    draw_panel(win->size_win, 0, "SIZE");
    draw_panel(win->time_win, 0, "DATA");
    int rows = 1;
    if (d)
    {
        while ((dir = readdir(d)) != NULL && rows < panel_height - 2)
        {
            if (strcmp(dir->d_name, ".") == 0)
                continue;
            if (win->file_count == win->current_now)
            {
                wattron(win->name_win, COLOR_PAIR(2));
                wattron(win->size_win, COLOR_PAIR(2));
                wattron(win->time_win, COLOR_PAIR(2));
            }

            mvwprintw(win->name_win, rows, 2, "%s", dir->d_name);

            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", ".", dir->d_name);
            if (stat(path, &st) == 0)
            {
                mvwprintw(win->size_win, rows, 1, "%ld", st.st_size);
                mvwprintw(win->time_win, rows, 1, "%s", ctime(&st.st_mtime));
                win->filelist[win->file_count].filename = strdup(dir->d_name);
                win->filelist[win->file_count].size = st.st_size;
                win->filelist[win->file_count].modify_time = strdup(ctime(&st.st_mtime));
                win->filelist[win->file_count].is_dir = S_ISDIR(st.st_mode);
            }
            if (win->file_count == win->current_now)
            {
                wattron(win->name_win, COLOR_PAIR(1));
                wattron(win->size_win, COLOR_PAIR(1));
                wattron(win->time_win, COLOR_PAIR(1));
            }
            rows++;
            win->file_count++;
        }
        closedir(d);
    }
    wrefresh(win->name_win);
    wrefresh(win->size_win);
    wrefresh(win->time_win);
}