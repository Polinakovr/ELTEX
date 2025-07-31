#include <fcntl.h>
#include <mqueue.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdatomic.h>
#include <errno.h>
#define MAX_NAME 15
#define MAX_MESSAGE 50
#define USER_NAME 20
#define MAX_CLIENT 10 

struct chat {
    char name[USER_NAME];
    char text[MAX_MESSAGE];
};

struct chat list[MAX_CLIENT];
atomic_uint user_count = 0;

WINDOW *win_message_users;
WINDOW *win_name_user;
WINDOW *win_entry_message;

struct chat my_client;
int message_pos = 2;
atomic_bool running = true;

void print_users() {
    werase(win_name_user);
    box(win_name_user, 0, 0);
    mvwprintw(win_name_user, 1, 2, "Users (%d):", user_count);

    for (unsigned int i = 0; i < user_count; i++) {
        mvwprintw(win_name_user, i + 2, 2, "%d: %s", i + 1, list[i].name);
    }

    wrefresh(win_name_user);
}

void print_message(const char *name, const char *message) {
    int max_y, max_x;
    getmaxyx(win_message_users, max_y, max_x);

    if (message_pos >= max_y - 1) {
        scroll(win_message_users);
        werase(win_message_users);
        box(win_message_users, 0, 0);
        mvwprintw(win_message_users, 1, 2, "Chat Messages:");
        message_pos = 2;
    }

    mvwprintw(win_message_users, message_pos, 2, "%s: %s", name, message);
    message_pos++;

    wrefresh(win_message_users);
}

void *text_handler(void *arg) {
    mqd_t message_client = mq_open("/message", O_WRONLY | O_NONBLOCK);
    if (message_client == (mqd_t)-1) {
        perror("Error opening message queue");
        exit(EXIT_FAILURE);
    }

    while (running) {
        wmove(win_entry_message, 2, 2);
        wclrtoeol(win_entry_message);
        mvwprintw(win_entry_message, 2, 2, "Message: ");
        wrefresh(win_entry_message);

        char messages[MAX_MESSAGE];
        echo();
        mvwgetnstr(win_entry_message, 2, 11, messages, MAX_MESSAGE);
        noecho();

        if (strcmp(messages, "exit") == 0) {
            running = false;
            break;
        }

        struct chat msg;
        strncpy(msg.name, my_client.name, USER_NAME);
        strncpy(msg.text, messages, MAX_MESSAGE);
        msg.text[MAX_MESSAGE - 1] = '\0';

        if (mq_send(message_client, (char *)&msg, sizeof(struct chat), 5) == -1) {
            perror("Error sending message");
            mq_close(message_client);
            exit(EXIT_FAILURE);
        }
        print_message(msg.name, msg.text);
    }

    mq_close(message_client);
    return NULL;
}

void *official_handler(void *arg) {
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 50,
        .mq_msgsize = sizeof(struct chat),
        .mq_curmsgs = 0
    };
    
    mqd_t official_queue = mq_open("/duty", O_RDONLY | O_NONBLOCK, 0666, &attr);
    if (official_queue == (mqd_t)-1) {
        perror("Error opening duty queue");
        exit(EXIT_FAILURE);
    }

    struct chat buffer;
    unsigned int priority;
    
    while (running) {
        ssize_t received = mq_receive(official_queue, (char *)&buffer, sizeof(struct chat), &priority);
        if (received == -1) {
            if (errno == EAGAIN) {
                usleep(100000);
                continue;
            }
            perror("Error receiving message");
            mq_close(official_queue);
            exit(EXIT_FAILURE);
        }

        if (priority == 6) { 
            if (user_count < MAX_CLIENT) {
                strncpy(list[user_count].name, buffer.name, USER_NAME);
                list[user_count].name[USER_NAME - 1] = '\0';
                memset(list[user_count].text, 0, MAX_MESSAGE);
                user_count++;
                print_users();
            }
        } else if (priority == 5) { 
            int found = 0;
            for (unsigned int i = 0; i < user_count; i++) {
                if (strcmp(list[i].name, buffer.name) == 0) {
                    strncpy(list[i].text, buffer.text, MAX_MESSAGE);
                    list[i].text[MAX_MESSAGE - 1] = '\0';
                    found = 1;
                    break;
                }
            }

            if (!found && user_count < MAX_CLIENT) {
                strncpy(list[user_count].name, buffer.name, USER_NAME);
                strncpy(list[user_count].text, buffer.text, MAX_MESSAGE);
                list[user_count].name[USER_NAME - 1] = '\0';
                list[user_count].text[MAX_MESSAGE - 1] = '\0';
                user_count++;
                print_users();
            }
            
            if (strcmp(buffer.name, my_client.name) != 0) {
                print_message(buffer.name, buffer.text);
            }
        }
    }

    mq_close(official_queue);
    return NULL;
}

int main() {
    initscr();
    curs_set(1);
    keypad(stdscr, TRUE);
    refresh();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    char my_name[MAX_NAME];
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(2, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(3, COLOR_MAGENTA, COLOR_CYAN);

    win_message_users = newwin(max_y - (max_y / 3), max_x - (max_x / 4), 0, 0);
    win_name_user = newwin(max_y - (max_y / 3), max_x / 4, 0, max_x - (max_x / 4));
    win_entry_message = newwin(max_y / 3, max_x, max_y - (max_y / 3), 0);

    wbkgd(win_message_users, COLOR_PAIR(1));
    wbkgd(win_name_user, COLOR_PAIR(2));
    wbkgd(win_entry_message, COLOR_PAIR(3));

    box(win_message_users, 0, 0);
    box(win_name_user, 0, 0);
    box(win_entry_message, 0, 0);

    mvwprintw(win_name_user, 1, (max_x / 4) / 2, "Users:");
    mvwprintw(win_message_users, 1, (max_x - (max_x / 4)) / 2, "Chat:");
    mvwprintw(win_entry_message, 1, 2, "User:");

    wrefresh(win_name_user);
    wrefresh(win_message_users);

    echo();
    mvwgetnstr(win_entry_message, 1, 9, my_name, MAX_NAME);
    noecho();

    strncpy(my_client.name, my_name, USER_NAME);
    my_client.name[USER_NAME - 1] = '\0';
    wrefresh(win_entry_message);

    mqd_t queue = mq_open("/name", O_WRONLY | O_NONBLOCK);
    if (queue == (mqd_t)-1) {
        perror("Error opening name queue");
        endwin();
        exit(EXIT_FAILURE);
    }

    if (mq_send(queue, (char *)&my_client, sizeof(struct chat), 6) == -1) {
        perror("Error sending name");
        mq_close(queue);
        endwin();
        exit(EXIT_FAILURE);
    }
    mq_close(queue);

    pthread_t official_thread, text_thread;
    if (pthread_create(&official_thread, NULL, official_handler, NULL) != 0) {
        perror("Error creating official thread");
        endwin();
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&text_thread, NULL, text_handler, NULL) != 0) {
        perror("Error creating text thread");
        pthread_cancel(official_thread);
        endwin();
        exit(EXIT_FAILURE);
    }

    pthread_join(text_thread, NULL);
    running = false;
    pthread_join(official_thread, NULL);

    delwin(win_message_users);
    delwin(win_entry_message);
    delwin(win_name_user);
    endwin();
    return 0;
}