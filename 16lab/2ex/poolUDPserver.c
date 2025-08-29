#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#define PORT 8081
#define SIZE_THREAD 10
#define SIZE 80
int socketfd;
typedef struct
{
    pthread_t thread;
    int flags;
    struct sockaddr_in users;
    socklen_t size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} threads;
int port;
threads pool[SIZE_THREAD];
void signal_handler(int sig)
{
    close(socketfd);
    for (int i = 0; i < SIZE_THREAD; i++)
    {
        pthread_cancel(pool[i].thread);
    }
    for (int i = 0; i < SIZE_THREAD; i++)
    {
        pthread_mutex_destroy(&pool[i].mutex);
        pthread_cond_destroy(&pool[i].cond);
    }
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
        }
        struct sockaddr_in client = Node->users;
        socklen_t client_size = Node->size;
        pthread_mutex_unlock(&Node->mutex);

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
        if ((sendto(newsocketfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, client_size)) == -1)
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
    sa.sa_handler = &signal_handler;
    sigaction(SIGINT, &sa, NULL);
    struct sockaddr_in server;
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    port = PORT;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    server.sin_port = htons(PORT);
    int opt = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    for (int i = 0; i < SIZE_THREAD; i++)
    {
        pool[i].flags = 0;
        pthread_mutex_init(&pool[i].mutex, NULL);
        pthread_cond_init(&pool[i].cond, NULL);
    }
    if (bind(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
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
        for (int i = 0; i < SIZE_THREAD; i++)
        {
            if (pool[i].flags == 0)
            {
                pool[i].flags = 1;
                pool[i].users = client;
                pool[i].size = client_len;

                printf("Увидел клиента %s:%d\n", inet_ntoa(pool[i].users.sin_addr), ntohs(client.sin_port));
                pthread_mutex_lock(&pool[i].mutex);
                pthread_cond_signal(&pool[i].cond);
                pthread_mutex_unlock(&pool[i].mutex);
                break;
            }
        }
    }
}
