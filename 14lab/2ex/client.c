#include <errno.h>
#include <fcntl.h>
#include <ncurses.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_NAME 15
#define MAX_MESSAGE 50
#define USER_NAME 20
#define MAX_CLIENT 10
#define SHM_N "/posix_serve_to_client_name"
#define SHM_M "/posix_serve_to_client_message"
#define SEM_NAME "/chat_sem"
#define SEM_COND "/chat_cond_sem"
#define SEM_BROAD "/chat_name_broadcast"
#define SEM_COND_T "/chat_cond_text"
#define SEM_LOCK_T "/chat_lock_text"
#define SEM_TEXT_BROADCAST "/chat_text_broadcast"
#define SHM_COUNT "/chat_user_count"
#define SEM_COUNT "/chat_sem_count_lock"
#define SEM_READ_CONFIRM "/chat_sem_read"

char my_name[MAX_NAME];
WINDOW *win_message_users = NULL;
WINDOW *win_name_user = NULL;
WINDOW *win_entry_message = NULL;
int message_pos = 2;
atomic_bool running = true;
pthread_mutex_t ncurses_mutex = PTHREAD_MUTEX_INITIALIZER;

struct message
{
    char text[MAX_MESSAGE];
};

struct users
{
    char name[USER_NAME];
    int priority;
};

int shm_fd = -1;
int shm = -1;
int shm_count_fd = -1;
unsigned int user_count = 0;
struct users *shm_name = NULL;
struct message *shm_message = NULL;
unsigned int *shm_count = NULL;
unsigned int my_index;
sem_t *sem_block_name = NULL;
sem_t *sem_cond_name = NULL;
sem_t *sem_cond_text = NULL;
sem_t *sem_text_block = NULL;
sem_t *sem_cond_broadcast = NULL;
sem_t *sem_text_broadcast = NULL;
sem_t *sem_user_count = NULL;
sem_t *sem_read_confirm = NULL;
void ncurses()
{
    initscr();
    if (!stdscr)
    {
        fprintf(stderr, "Ошибка инициализации ncurses\n");
        exit(EXIT_FAILURE);
    }
    curs_set(1);
    keypad(stdscr, TRUE);
    timeout(100); // Таймаут для ввода, чтобы избежать блокировки
    refresh();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(2, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(3, COLOR_MAGENTA, COLOR_CYAN);

    win_message_users = newwin(max_y - (max_y / 3), max_x - (max_x / 4), 0, 0);
    win_name_user = newwin(max_y - (max_y / 3), max_x / 4, 0, max_x - (max_x / 4));
    win_entry_message = newwin(max_y / 3, max_x, max_y - (max_y / 3), 0);

    if (!win_message_users || !win_name_user || !win_entry_message)
    {
        fprintf(stderr, "Ошибка создания окон ncurses\n");
        endwin();
        exit(EXIT_FAILURE);
    }

    wbkgd(win_message_users, COLOR_PAIR(1));
    wbkgd(win_name_user, COLOR_PAIR(2));
    wbkgd(win_entry_message, COLOR_PAIR(3));

    box(win_message_users, 0, 0);
    box(win_name_user, 0, 0);
    box(win_entry_message, 0, 0);

    pthread_mutex_lock(&ncurses_mutex);
    mvwprintw(win_name_user, 1, (max_x / 4) / 2, "Users:");
    mvwprintw(win_message_users, 1, (max_x - (max_x / 4)) / 2, "Chat:");
    mvwprintw(win_entry_message, 1, 2, "User:");
    wrefresh(win_name_user);
    wrefresh(win_message_users);
    pthread_mutex_unlock(&ncurses_mutex);

    echo();
    mvwgetnstr(win_entry_message, 1, 9, my_name, MAX_NAME);
    noecho();

    pthread_mutex_lock(&ncurses_mutex);
    wrefresh(win_entry_message);
    pthread_mutex_unlock(&ncurses_mutex);
}

void clean()
{
    if (sem_block_name)
        sem_close(sem_block_name);
    if (sem_user_count)
        sem_close(sem_user_count);
    if (sem_cond_name)
        sem_close(sem_cond_name);
    if (sem_text_block)
        sem_close(sem_text_block);
    if (sem_cond_text)
        sem_close(sem_cond_text);
    if (sem_cond_broadcast)
        sem_close(sem_cond_broadcast);
    if (sem_text_broadcast)
        sem_close(sem_text_broadcast);
    if (sem_read_confirm)
        sem_close(sem_read_confirm);
    if (shm_name)
        munmap(shm_name, sizeof(struct users) * MAX_CLIENT);
    if (shm_message)
        munmap(shm_message, sizeof(struct message) * MAX_CLIENT);
    if (shm_count)
        munmap(shm_count, sizeof(unsigned int));
    if (shm_fd >= 0)
    {
        close(shm_fd);
    }
    if (shm >= 0)
    {
        close(shm);
    }
    if (shm_count_fd >= 0)
    {
        close(shm_count_fd);
    }
    if (win_message_users)
        delwin(win_message_users);
    if (win_name_user)
        delwin(win_name_user);
    if (win_entry_message)
        delwin(win_entry_message);
    endwin();
    pthread_mutex_destroy(&ncurses_mutex);
}

void shm_init()
{
    shm_fd = shm_open(SHM_N, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        fprintf(stderr, "Ошибка открытия разделяемой памяти %s: %s\n", SHM_N, strerror(errno));
        clean();
        exit(EXIT_FAILURE);
    }
    shm_name = mmap(NULL, sizeof(struct users) * MAX_CLIENT, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_name == MAP_FAILED)
    {
        fprintf(stderr, "Ошибка отображения разделяемой памяти %s: %s\n", SHM_N, strerror(errno));
        close(shm_fd);
        clean();
        exit(EXIT_FAILURE);
    }
    shm_count_fd = shm_open(SHM_COUNT, O_RDWR, 0666);
    if (shm_count_fd == -1)
    {
        fprintf(stderr, "Ошибка открытия разделяемой памяти %s: %s\n", SHM_COUNT, strerror(errno));
        munmap(shm_name, sizeof(struct users) * MAX_CLIENT);
        close(shm_fd);
        clean();
        exit(EXIT_FAILURE);
    }
    shm_count = mmap(NULL, sizeof(unsigned int), PROT_WRITE | PROT_READ, MAP_SHARED, shm_count_fd, 0);
    if (shm_count == MAP_FAILED)
    {
        fprintf(stderr, "Ошибка отображения разделяемой памяти %s: %s\n", SHM_COUNT, strerror(errno));
        close(shm_count_fd);
        munmap(shm_name, sizeof(struct users) * MAX_CLIENT);
        close(shm_fd);
        clean();
        exit(EXIT_FAILURE);
    }
    shm = shm_open(SHM_M, O_RDWR, 0666);
    if (shm == -1)
    {
        fprintf(stderr, "Ошибка открытия разделяемой памяти %s: %s\n", SHM_M, strerror(errno));
        munmap(shm_name, sizeof(struct users) * MAX_CLIENT);
        munmap(shm_count, sizeof(unsigned int));
        close(shm_fd);
        close(shm_count_fd);
        clean();
        exit(EXIT_FAILURE);
    }
    sem_read_confirm = sem_open(SEM_READ_CONFIRM, 0);
    if (sem_read_confirm == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_READ_CONFIRM, strerror(errno));
        exit(EXIT_FAILURE);
    }
    shm_message = mmap(NULL, sizeof(struct message) * MAX_CLIENT, PROT_WRITE | PROT_READ, MAP_SHARED, shm, 0);
    if (shm_message == MAP_FAILED)
    {
        fprintf(stderr, "Ошибка отображения разделяемой памяти %s: %s\n", SHM_M, strerror(errno));
        munmap(shm_name, sizeof(struct users) * MAX_CLIENT);
        munmap(shm_count, sizeof(unsigned int));
        close(shm_fd);
        close(shm_count_fd);
        close(shm);
        clean();
        exit(EXIT_FAILURE);
    }
    sem_block_name = sem_open(SEM_NAME, 0);
    if (sem_block_name == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_NAME, strerror(errno));
        clean();
        exit(EXIT_FAILURE);
    }
    sem_cond_name = sem_open(SEM_COND, 0);
    if (sem_cond_name == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_COND, strerror(errno));
        clean();
        exit(EXIT_FAILURE);
    }
    sem_text_block = sem_open(SEM_LOCK_T, 0);
    if (sem_text_block == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_LOCK_T, strerror(errno));
        clean();
        exit(EXIT_FAILURE);
    }
    sem_cond_text = sem_open(SEM_COND_T, 0);
    if (sem_cond_text == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_COND_T, strerror(errno));
        clean();
        exit(EXIT_FAILURE);
    }
    sem_text_broadcast = sem_open(SEM_TEXT_BROADCAST, 0);
    if (sem_text_broadcast == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_TEXT_BROADCAST, strerror(errno));
        clean();
        exit(EXIT_FAILURE);
    }
    sem_cond_broadcast = sem_open(SEM_BROAD, 0);
    if (sem_cond_broadcast == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_BROAD, strerror(errno));
        clean();
        exit(EXIT_FAILURE);
    }
    sem_user_count = sem_open(SEM_COUNT, 0);
    if (sem_user_count == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_COUNT, strerror(errno));
        clean();
        exit(EXIT_FAILURE);
    }
}

void print_users()
{
    if (!win_name_user)
    {
        fprintf(stderr, "Ошибка: win_name_user равен NULL\n");
        return;
    }
    pthread_mutex_lock(&ncurses_mutex);
    sem_wait(sem_user_count);
    user_count = *shm_count;
    sem_post(sem_user_count);
    werase(win_name_user);
    box(win_name_user, 0, 0);
    mvwprintw(win_name_user, 1, 2, "Users (%d):", user_count);
    for (unsigned int i = 0; i < MAX_CLIENT; i++)
    {
        if (shm_name[i].name[0] != '\0' && shm_name[i].priority == 1)
        {
            mvwprintw(win_name_user, i + 2, 2, "%d: %s", i + 1, shm_name[i].name);
        }
    }
    wrefresh(win_name_user);
    pthread_mutex_unlock(&ncurses_mutex);
}

void print_message(const char *name, const char *message)
{
    if (!win_message_users)
    {
        fprintf(stderr, "Ошибка: win_message_users равен NULL\n");
        return;
    }
    pthread_mutex_lock(&ncurses_mutex);
    sem_wait(sem_text_block);
    int max_y, max_x;
    getmaxyx(win_message_users, max_y, max_x);
    if (message_pos >= max_y - 1)
    {
        scroll(win_message_users);
        werase(win_message_users);
        box(win_message_users, 0, 0);
        mvwprintw(win_message_users, 1, 2, "Chat Messages:");
        message_pos = 2;
    }
    mvwprintw(win_message_users, message_pos, 2, "%s: %s", name, message);
    message_pos++;
    wrefresh(win_message_users);
    sem_post(sem_text_block);
    pthread_mutex_unlock(&ncurses_mutex);
}

void *text_handler(void *arg)
{
    while (running)
    {
        if (!win_entry_message)
        {
            fprintf(stderr, "Ошибка: win_entry_message равен NULL\n");
            break;
        }
        pthread_mutex_lock(&ncurses_mutex);
        wmove(win_entry_message, 2, 2);
        wclrtoeol(win_entry_message);
        mvwprintw(win_entry_message, 2, 2, "Message: ");
        wrefresh(win_entry_message);
        pthread_mutex_unlock(&ncurses_mutex);

        char messages[MAX_MESSAGE];
        echo();
        mvwgetnstr(win_entry_message, 2, 11, messages, MAX_MESSAGE);
        noecho();

        if (strcmp(messages, "exit") == 0)
        {
            running = false;
            sem_wait(sem_block_name);
            shm_name[my_index].priority = 3; // Пометить пользователя как вышедшего
            sem_post(sem_block_name);
            sem_post(sem_cond_name); // Уведомить обработчик имен
            sem_wait(sem_user_count);
            unsigned int uc = *shm_count;

            sem_post(sem_user_count);

            for (unsigned int i = 0; i < (uc + 2); i++)
            {
                sem_post(sem_cond_broadcast);
                sem_post(sem_text_broadcast);
            }

            break;
        }
        sem_wait(sem_text_block);
        for (unsigned int i = 0; i < MAX_CLIENT; i++)
        {
            if (strcmp(shm_name[i].name, my_name) == 0 && shm_name[i].priority == 1)
            {
                strncpy(shm_message[i].text, messages, MAX_MESSAGE - 1);
                shm_message[i].text[MAX_MESSAGE - 1] = '\0';

                break;
            }
        }

        sem_post(sem_cond_text);
        sem_post(sem_text_block);
    }
    return NULL;
}

void *message_listener(void *arg)
{
    while (running)
    {
        sem_wait(sem_text_broadcast);
        if (!running)
        {
            sem_post(sem_read_confirm);
            break;
        }
        sem_wait(sem_user_count);
        user_count = *shm_count;
        sem_post(sem_user_count);

        for (unsigned int i = 0; i < user_count; i++)
        {
            if (shm_name[i].priority == 1 && strlen(shm_message[i].text) > 0)
            {
                print_message(shm_name[i].name, shm_message[i].text);
                break;
            }
        }
        sem_post(sem_read_confirm);
    }
    return NULL;
}

void *official_handler(void *arg)
{
    sem_wait(sem_block_name);
    sem_wait(sem_user_count);
    user_count = *shm_count;
    my_index = user_count;
    sem_post(sem_user_count);
    if (user_count >= MAX_CLIENT)
    {
        fprintf(stderr, "Максимальное количество клиентов достигнуто\n");
        sem_post(sem_block_name);
        return NULL;
    }
    strncpy(shm_name[my_index].name, my_name, USER_NAME - 1);
    shm_name[my_index].name[USER_NAME - 1] = '\0';
    shm_name[my_index].priority = 6;
    sem_post(sem_block_name);
    sem_post(sem_cond_name);
    print_users();
    while (running)
    {
        sem_wait(sem_cond_broadcast);
        print_users();
    }
    return NULL;
}

int main()
{
    ncurses();
    shm_init();
    pthread_t official_thread, text_thread, message_thread;
    if (pthread_create(&official_thread, NULL, official_handler, NULL) != 0)
    {
        fprintf(stderr, "Ошибка создания потока official: %s\n", strerror(errno));
        clean();
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&text_thread, NULL, text_handler, NULL) != 0)
    {
        fprintf(stderr, "Ошибка создания потока text: %s\n", strerror(errno));
        pthread_cancel(official_thread);
        clean();
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&message_thread, NULL, message_listener, NULL) != 0)
    {
        fprintf(stderr, "Ошибка создания потока message listener: %s\n", strerror(errno));
        pthread_cancel(official_thread);
        pthread_cancel(text_thread);
        clean();
        exit(EXIT_FAILURE);
    }
    pthread_join(text_thread, NULL);
    pthread_join(official_thread, NULL);
    pthread_join(message_thread, NULL);
    running = false;
    clean();
    return 0;
}