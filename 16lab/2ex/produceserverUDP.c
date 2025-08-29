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

#define PORT 8081
#define SIZE_THREAD 10
#define SIZE 80
#define TASK 10
int port;
int socketfd;
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct Node
{
    struct Node *prev;
    struct Node *next;
    struct sockaddr_in client;
} Node;

Node *head = NULL;
Node *tail = NULL;

typedef struct
{
    pthread_t thread;
        pthread_mutex_t mutex;
    pthread_cond_t cond;
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
        free(temp);
    }
    head = NULL;
    tail = NULL;
    pthread_mutex_unlock(&list_mutex);
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

void add(struct sockaddr_in newsocketfd)
{
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL)
    {
        printf("Ошибка выделения памяти: %s\n", strerror(errno));
        return;
    }
    new_node->client = newsocketfd;
    new_node->next = NULL;

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
}

struct sockaddr_in pop()
{
    if (head == NULL)
    {
        pthread_mutex_unlock(&list_mutex);
        exit(EXIT_FAILURE);
    }
    Node *temp = head;
    struct sockaddr_in newclient = temp->client;
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
    return newclient;
}

void *handler_client(void *arg)
{
    while (1)
    {
        char buffer[SIZE];
        threads *Node = (threads *)arg;

         pthread_mutex_lock(&Node->mutex);
        while (Node->flags == 0)
        {
          pthread_cond_wait(&Node->cond, &Node->mutex);
             printf("Сигнал\n");
        }
        printf("Сигнал получен\n");
  struct sockaddr_in client = pop();
         pthread_mutex_unlock(&Node->mutex);
      
        printf("Увидел клиента %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        printf("%d\n", Node->flags);

        int newsocketfd;
        struct sockaddr_in newserver;
        if ((newsocketfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            printf("%s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        memset(&newserver, 0, sizeof(newserver));
        newserver.sin_family = AF_INET;
        inet_pton(AF_INET, "127.0.0.1", &newserver.sin_addr);
        port++;
        newserver.sin_port = htons(port);
        if (bind(newsocketfd, (struct sockaddr *)&newserver, sizeof(newserver)) == -1)
        {
            printf("%s", strerror(errno));
            close(newsocketfd);
            exit(EXIT_FAILURE);
        }
        str_time(buffer);
        printf("Увидел клиента %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        socklen_t client_len = sizeof(client);
        if ((sendto(newsocketfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, client_len)) == -1)
        {
            printf("%s", strerror(errno));
        }
        printf("Обработал нового клиента\n");
     pthread_mutex_lock(&Node->mutex);
        Node->flags = 0;
        pthread_mutex_unlock(&Node->mutex);
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
        pthread_mutex_init(&pool[i].mutex, NULL);
        pthread_cond_init(&pool[i].cond, NULL);
    }
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
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
    port = PORT;
    server.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    server.sin_port = htons(PORT);

    if (bind(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("Ошибка bind: %s\n", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на порту %d\n", PORT);

    for (int i = 0; i < SIZE_THREAD; i++)
    {
        if (pool[i].flags == 0)
        {
            pthread_create(&pool[i].thread, NULL, &handler_client, (void *)&pool[i]);
        }
    }
    while (1)
    {
        char buffer[SIZE] = "\0";
        struct sockaddr_in client;
        memset(&client, 0, sizeof(client));
        socklen_t client_len = sizeof(client);
        if (recvfrom(socketfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &client_len) == -1)
        {
            printf("%s", strerror(errno));
            close(socketfd);
            exit(EXIT_FAILURE);
        }
        printf("Получил ответ \n");
        add(client);
        for (int i = 0; i < SIZE_THREAD; i++)
        {
            if (pool[i].flags == 0)
            {
                pool[i].flags = 1;
                pthread_mutex_lock(&pool[i].mutex);
                pthread_cond_signal(&pool[i].cond);
                pthread_mutex_unlock(&pool[i].mutex);
                break;
            }
        }

        printf("Соединение добавлено в очередь\n");
    }

    close(socketfd);
    return 0;
}