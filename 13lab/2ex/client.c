#include <fcntl.h> /* For O_* constants */
#include <mqueue.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> /* For mode constants */
#include <sys/types.h>
#include <unistd.h>
#define MAX_NAME 15

void official_func(void)
{
    unsigned int count=0;
    mqd_t duty_queue = mq_open("/duty", O_RDONLY);
    if (duty_queue == (mqd_t)-1)
    {
        perror("Error create queue");
        exit(EXIT_FAILURE);
    }
    unsigned int prio;
    char name = realloc(name,sizeof(char)*(count+1));
    if (mq_receive(duty_queue, buffer, MAX_NAME, &prio) == -1)
    {
        perror("Error receive message");
        exit(EXIT_FAILURE);
    }
    if (prio == 6)
    {   
        count++;
         name = realloc(name,sizeof(char)*(count+1)); /*реализовывать через стркутуру для хранения имен как в сервере*/
    }
}
int main()
{
    initscr();
    curs_set(1);
    refresh();
    char name[MAX_NAME + 1];
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(2, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(3, COLOR_MAGENTA, COLOR_CYAN);
    WINDOW *win_message_users = newwin(max_y - (max_y / 3), max_x - (max_x / 4), 0, 0);
    WINDOW *win_name_users = newwin(max_y - (max_y / 3), max_x / 4, 0, max_x - (max_x / 4));
    WINDOW *win_entry_message = newwin(max_y - (max_y / 3), max_x, max_y - (max_y / 3), 0);
    wbkgd(win_message_users, COLOR_PAIR(1));

    wbkgd(win_name_users, COLOR_PAIR(2));
    wbkgd(win_entry_message, COLOR_PAIR(3));

    box(win_message_users, 0, 0);
    box(win_name_users, 0, 0);
    box(win_entry_message, 0, 0);
    wrefresh(win_message_users);
    wrefresh(win_name_users);
    mqd_t queue = mq_open("/name", O_WRONLY);
    if (queue == (mqd_t)-1)
    {
        perror("Error create queue");
        exit(EXIT_FAILURE);
    }
    mvwprintw(win_name_users, 1, (max_x / 4) / 2, "Users:");
    mvwprintw(win_message_users, 1, (max_x - (max_x / 4)) / 2, "Chat:");
    wrefresh(win_name_users);
    wrefresh(win_message_users);
    mvwprintw(win_entry_message, 1, 2, "Users:");
    mvwgetnstr(win_entry_message, 1, 9, name, MAX_NAME);
    wrefresh(win_entry_message);
    mvwprintw(win_entry_message, 2, 2, "Message:");
    wrefresh(win_entry_message);
    if (mq_send(queue, name, sizeof(name), 6) == -1)
    {
        perror("Error send queue");
        exit(EXIT_FAILURE);
    }
    phtread_t official;
    phtread_t send_chat;
    pthread_create(&official, NULL, /*ФУНКЦ*/, NULL);
    pthread_create(&send_chat, NULL, /*ФУНКЦ*/, NULL);
    refresh();
    getch();
    delwin(win_message_users);
    delwin(win_entry_message);
    delwin(win_name_users);
    endwin();
}
