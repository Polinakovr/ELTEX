#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PORT 8080
#define SIZE_THREAD 10
#define SIZE 80
#define TASK 10

int socketfd;
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;
typedef struct Node
{
    struct Node *prev;
    struct Node *next;
    int newsocketfd;
} Node;

Node *head = NULL;
Node *tail = NULL;

typedef struct
{
    pthread_t thread;
    int flags;
} threads;

threads pool[SIZE_THREAD];

void signal_handler(int sig)
{
    close(socketfd);
    pthread_mutex_lock(&list_mutex);
    Node *current = head;
    while (current != NULL)
    {
        Node *temp = current;
        current = current->next;
        close(temp->newsocketfd);
        free(temp);
    }
    head = NULL;
    tail = NULL;
    pthread_mutex_unlock(&list_mutex);
     for (int i = 0; i < SIZE_THREAD; i++)
    {
        pthread_cancel(pool[i].thread);
    }
  
    printf("Очистка\n");
    exit(EXIT_SUCCESS);
}

void str_time(char *buffer)
{
    time_t rawTime;
    struct tm *timeinfo;
    time(&rawTime);
    timeinfo = localtime(&rawTime);
    strftime(buffer, SIZE, "%Y-%m-%d %H:%M:%S", timeinfo);
}

void add(int newsocketfd)
{
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL)
    {
        printf("Ошибка выделения памяти: %s\n", strerror(errno));
        close(newsocketfd);
        return;
    }
    new_node->newsocketfd = newsocketfd;
    new_node->next = NULL;

    pthread_mutex_lock(&list_mutex);
    if (head == NULL)
    {
        new_node->prev = NULL;
        head = new_node;
        tail = new_node;
    }
    else
    {
        new_node->prev = tail;
        tail->next = new_node;
        tail = new_node;
    }
    pthread_mutex_unlock(&list_mutex);
}

int pop()
{
    pthread_mutex_lock(&list_mutex);
    if (head == NULL)
    {
        pthread_mutex_unlock(&list_mutex);
        return -1;
    }

    Node *temp = head;
    int newsocketfd = temp->newsocketfd;
    head = head->next;

    if (head == NULL)
    {
        tail = NULL;
    }
    else
    {
        head->prev = NULL;
    }

    free(temp);
    pthread_mutex_unlock(&list_mutex);
    return newsocketfd;
}

void *handler_client(void *arg)
{
    char buffer[SIZE];
    while (1)
    {
        pthread_mutex_lock(&list_mutex);

        while (head == NULL)
        {
            pthread_cond_wait(&queue_cond, &list_mutex);
        }
        pthread_mutex_unlock(&list_mutex);
        int newsocketfd = pop();

        str_time(buffer);
        printf("Отправляем время клиенту (сокет %d)\n", newsocketfd);
        if (send(newsocketfd, buffer, strlen(buffer) + 1, 0) == -1)
        {
            printf("Ошибка send: %s\n", strerror(errno));
        }
        close(newsocketfd);
    }

    return NULL;
}

int main()
{
    struct sigaction sa;
    struct sockaddr_in server, client;

    sa.sa_handler = &signal_handler;
    sigaction(SIGINT, &sa, NULL);

    for (int i = 0; i < SIZE_THREAD; i++)
    {
        pool[i].flags = 0;
    }
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Ошибка socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        printf("Ошибка setsockopt: %s\n", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("Ошибка bind: %s\n", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    if (listen(socketfd, 5) == -1)
    {
        printf("Ошибка listen: %s\n", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на порту %d\n", PORT);

    for (int i = 0; i < SIZE_THREAD; i++)
    {
        pthread_mutex_lock(&list_mutex);
        if (pool[i].flags == 0)
        {
            pool[i].flags = 1;
            if (pthread_create(&pool[i].thread, NULL, handler_client, &pool[i]) != 0)
            {
                printf("Ошибка pthread_create: %s\n", strerror(errno));
                pool[i].flags = 0;
            }
            else
            {
                pthread_detach(pool[i].thread);
            }
        }
        pthread_mutex_unlock(&list_mutex);
    }

    while (1)
    {
        socklen_t client_len = sizeof(client);
        int newsocket = accept(socketfd, (struct sockaddr *)&client, &client_len);
        if (newsocket == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            printf("Ошибка accept: %s\n", strerror(errno));
            continue;
        }

        printf("Соединение установлено с клиентом (сокет %d)\n", newsocket);

        add(newsocket);
        pthread_cond_signal(&queue_cond);
        printf("Соединение добавлено в очередь\n");
    }

    close(socketfd);
    return 0;
}