#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
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
struct users *shm_name = NULL;
struct message *shm_message = NULL;
unsigned int *shm_count = 0L;
unsigned int count = 0;
sem_t *sem_block_name = NULL;
sem_t *sem_cond_name = NULL;
sem_t *sem_cond_text = NULL;
sem_t *sem_text_block = NULL;
sem_t *sem_cond_broadcast = NULL;
sem_t *sem_text_broadcast = NULL;
sem_t *sem_user_count = NULL;
sem_t *sem_read_confirm = NULL;
volatile sig_atomic_t keep_running = 1;

void handle_signal(int sig)
{
    printf("Получен сигнал для завершения работы\n");
    keep_running = 0;
    sem_post(sem_cond_name);
    sem_post(sem_cond_text);
    sem_post(sem_block_name);
    sem_post(sem_user_count);
    sem_post(sem_text_block);
    sem_post(sem_cond_broadcast);
    sem_post(sem_text_broadcast);
    sem_post(sem_read_confirm);
}

void shm_init()
{
    shm_fd = shm_open(SHM_N, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        fprintf(stderr, "Ошибка создания разделяемой памяти %s: %s\n", SHM_N, strerror(errno));
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd, sizeof(struct users) * MAX_CLIENT);
    shm_name = mmap(NULL, sizeof(struct users) * MAX_CLIENT, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_name == MAP_FAILED)
    {
        fprintf(stderr, "Ошибка отображения разделяемой памяти %s: %s\n", SHM_N, strerror(errno));
        close(shm_fd);
        exit(EXIT_FAILURE);
    }
    shm = shm_open(SHM_M, O_CREAT | O_RDWR, 0666);
    if (shm == -1)
    {
        fprintf(stderr, "Ошибка создания разделяемой памяти %s: %s\n", SHM_M, strerror(errno));
        munmap(shm_name, sizeof(struct users) * MAX_CLIENT);
        shm_unlink(SHM_N);
        exit(EXIT_FAILURE);
    }
    ftruncate(shm, sizeof(struct message) * MAX_CLIENT);
    shm_message = mmap(NULL, sizeof(struct message) * MAX_CLIENT, PROT_WRITE | PROT_READ, MAP_SHARED, shm, 0);
    if (shm_message == MAP_FAILED)
    {
        fprintf(stderr, "Ошибка отображения разделяемой памяти %s: %s\n", SHM_M, strerror(errno));
        munmap(shm_name, sizeof(struct users) * MAX_CLIENT);
        shm_unlink(SHM_N);
        close(shm);
        exit(EXIT_FAILURE);
    }
    shm_count_fd = shm_open(SHM_COUNT, O_CREAT | O_RDWR, 0666);
    if (shm_count_fd == -1)
    {
        fprintf(stderr, "Ошибка открытия разделяемой памяти %s: %s\n", SHM_COUNT, strerror(errno));
        munmap(shm_name, sizeof(struct users) * MAX_CLIENT);
        munmap(shm_message, sizeof(struct message) * MAX_CLIENT);
        shm_unlink(SHM_N);
        shm_unlink(SHM_M);
        close(shm_fd);
        close(shm);
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_count_fd, sizeof(unsigned int));
    shm_count = mmap(NULL, sizeof(unsigned int), PROT_WRITE | PROT_READ, MAP_SHARED, shm_count_fd, 0);
    if (shm_count == MAP_FAILED)
    {
        fprintf(stderr, "Ошибка отображения разделяемой памяти %s: %s\n", SHM_COUNT, strerror(errno));
        close(shm_count_fd);
        munmap(shm_name, sizeof(struct users) * MAX_CLIENT);
        munmap(shm_message, sizeof(struct message) * MAX_CLIENT);
        shm_unlink(SHM_N);
        shm_unlink(SHM_M);
        close(shm_fd);
        close(shm);
        exit(EXIT_FAILURE);
    }
    sem_read_confirm = sem_open(SEM_READ_CONFIRM, O_CREAT, 0666, 0);
    if (sem_read_confirm == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_READ_CONFIRM, strerror(errno));
        exit(EXIT_FAILURE);
    }
    sem_block_name = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem_block_name == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_NAME, strerror(errno));
        exit(EXIT_FAILURE);
    }
    sem_cond_name = sem_open(SEM_COND, O_CREAT, 0666, 0);
    if (sem_cond_name == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_COND, strerror(errno));
        exit(EXIT_FAILURE);
    }
    sem_text_block = sem_open(SEM_LOCK_T, O_CREAT, 0666, 1);
    if (sem_text_block == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_LOCK_T, strerror(errno));
        exit(EXIT_FAILURE);
    }
    sem_cond_text = sem_open(SEM_COND_T, O_CREAT, 0666, 0);
    if (sem_cond_text == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_COND_T, strerror(errno));
        exit(EXIT_FAILURE);
    }
    sem_cond_broadcast = sem_open(SEM_BROAD, O_CREAT, 0666, 0);
    if (sem_cond_broadcast == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_BROAD, strerror(errno));
        exit(EXIT_FAILURE);
    }
    sem_text_broadcast = sem_open(SEM_TEXT_BROADCAST, O_CREAT, 0666, 0);
    if (sem_text_broadcast == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_TEXT_BROADCAST, strerror(errno));
        exit(EXIT_FAILURE);
    }
    sem_user_count = sem_open(SEM_COUNT, O_CREAT, 0666, 1);
    if (sem_user_count == SEM_FAILED)
    {
        fprintf(stderr, "Ошибка открытия семафора %s: %s\n", SEM_COUNT, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void clean()
{
    if (sem_block_name)
        sem_close(sem_block_name);
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
    if (sem_user_count)
        sem_close(sem_user_count);
    if (shm_name)
        munmap(shm_name, sizeof(struct users) * MAX_CLIENT);
    if (shm_message)
        munmap(shm_message, sizeof(struct message) * MAX_CLIENT);
    if (shm_count)
        munmap(shm_count, sizeof(unsigned int));
    if (shm_fd >= 0)
    {
        close(shm_fd);
        shm_unlink(SHM_N);
    }
    if (shm >= 0)
    {
        close(shm);
        shm_unlink(SHM_M);
    }
    if (shm_count_fd >= 0)
    {
        close(shm_count_fd);
        shm_unlink(SHM_COUNT);
    }
    sem_unlink(SEM_NAME);
    sem_unlink(SEM_COND);
    sem_unlink(SEM_LOCK_T);
    sem_unlink(SEM_COND_T);
    sem_unlink(SEM_BROAD);
    sem_unlink(SEM_TEXT_BROADCAST);
    sem_unlink(SEM_COUNT);
    sem_unlink(SEM_READ_CONFIRM);
}

void *name_handler(void *arg)
{
    while (keep_running)
    {
        printf("Сервер: Ожидание уведомления о новом пользователе...\n");
        sem_wait(sem_cond_name);
        sem_wait(sem_block_name);
        printf("Сервер: Получено уведомление о новом пользователе\n");
        sem_wait(sem_user_count);
        count = *shm_count;
        for (int i = 0; i < MAX_CLIENT; i++)
        {
            if (shm_name[i].priority == 6)
            {
                printf("Сервер: Найден новый пользователь на индексе %d: %s\n", i, shm_name[i].name);
                shm_name[i].priority = 1;
                (*shm_count)++;
                count = *shm_count;
                for (int j = 0; j < count; j++)
                {
                    sem_post(sem_cond_broadcast);
                }
                printf("Сервер: Уведомлены %u клиентов о новом пользователе\n", count);
                break;
            }
            else if (shm_name[i].priority == 3)
            {
                printf("Сервер: Пользователь %s на индексе %d вышел\n", shm_name[i].name, i);
                shm_name[i].priority = 0;
                shm_name[i].name[0] = '\0';
                for (int j = i; j < MAX_CLIENT - 1; j++)
                {
                    if (shm_name[j + 1].priority == 1)
                    {
                        memcpy(&shm_name[j], &shm_name[j + 1], sizeof(struct users));
                        memcpy(&shm_message[j], &shm_message[j + 1], sizeof(struct message));
                        shm_name[j].priority = 1;
                        shm_name[j + 1].priority = 0;
                        shm_name[j + 1].name[0] = '\0';
                        memset(shm_message[j + 1].text, 0, MAX_MESSAGE);
                    }
                }
                (*shm_count)--;
                count = *shm_count;
                for (int j = 0; j < count; j++)
                {
                    sem_post(sem_cond_broadcast);
                }
                printf("Сервер: Уведомлены %u клиентов об удалении пользователя\n", count);
                break;
            }
        }
        sem_post(sem_user_count);
        sem_post(sem_block_name);
    }
    printf("Сервер: Обработчик сообщений завершает работу\n");
    return NULL;
}
void *text_handler(void *arg)
{
    while (keep_running)
    {
        sem_wait(sem_cond_text);
        printf("Сервер: Получено новое сообщение\n");

        sem_wait(sem_user_count);
        count = *shm_count;
        sem_post(sem_user_count);
        for (int i = 0; i < count; i++)
        {
            if (shm_name[i].priority == 1 && strlen(shm_message[i].text) > 0)
            {
                printf("Сервер: Рассылка сообщения от %s: %s\n", shm_name[i].name, shm_message[i].text);
                for (int j = 0; j < count; j++)
                {
                    sem_post(sem_text_broadcast);
                }
                for (int k = 0; k < count; k++)
                {
                    sem_wait(sem_read_confirm);
                }
                memset(shm_message[i].text, 0, sizeof(shm_message[i].text));
                break;
            }
        }
    }
    printf("Сервер: Обработчик сообщений завершает работу\n");
    return NULL;
}
int main()
{
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    shm_init();
    pthread_t text_thread, name_thread;
    if (pthread_create(&text_thread, NULL, text_handler, NULL) != 0)
    {
        fprintf(stderr, "Ошибка создания потока text: %s\n", strerror(errno));
        clean();
        return EXIT_FAILURE;
    }
    if (pthread_create(&name_thread, NULL, name_handler, NULL) != 0)
    {
        fprintf(stderr, "Ошибка создания потока name: %s\n", strerror(errno));
        pthread_cancel(text_thread);
        clean();
        return EXIT_FAILURE;
    }
    pthread_join(text_thread, NULL);
    pthread_join(name_thread, NULL);
    clean();
    return 0;
}