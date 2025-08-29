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

#define PORT 8080
#define SIZE_THREAD 10
#define SIZE 80

int socketfd;

typedef struct
{
    pthread_t thread;
    int newsocket;
    int flags;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} threads;

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
    threads *client_sock = (threads *)arg;

    while (1)
    {
        pthread_mutex_lock(&client_sock->mutex);

        while (client_sock->flags == 0)
        {
            pthread_cond_wait(&client_sock->cond, &client_sock->mutex);
        }

        pthread_mutex_unlock(&client_sock->mutex);

        char buffer[SIZE];
        str_time(buffer);

        if ((send(client_sock->newsocket, buffer, sizeof(buffer), 0)) == -1)
        {
            printf("Error %s \n", strerror(errno));
        }

        close(client_sock->newsocket);

        pthread_mutex_lock(&client_sock->mutex);
        client_sock->flags = 0;
        pthread_mutex_unlock(&client_sock->mutex);
    }
    return NULL;
}

int main()
{
    struct sigaction sa;
    struct sockaddr_in server, client;

    for (int i = 0; i < SIZE_THREAD; i++)
    {
        pool[i].flags = 0;
        pthread_mutex_init(&pool[i].mutex, NULL);
        pthread_cond_init(&pool[i].cond, NULL);
    }

    server.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    sa.sa_handler = &signal_handler;
    sigaction(SIGINT, &sa, NULL);
    server.sin_port = htons(PORT);

    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Error:%s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("Error 1:%s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (listen(socketfd, 100) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < SIZE_THREAD; i++)
    {
        pthread_create(&pool[i].thread, NULL, &handler_client, (void *)&pool[i]);
    }

    while (1)
    {
        int newsocket;
        socklen_t client_len = sizeof(client);
        newsocket = accept(socketfd, (struct sockaddr *)&client, &client_len);

        if (newsocket == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            printf("Accept error: %s\n", strerror(errno));
            continue;
        }
        for (int i = 0; i < SIZE_THREAD; i++)
        {
            pthread_mutex_lock(&pool[i].mutex);

            if (pool[i].flags == 0)
            {
                pool[i].flags = 1;
                pool[i].newsocket = newsocket;

                pthread_cond_signal(&pool[i].cond);

                pthread_mutex_unlock(&pool[i].mutex);

                break;
            }

            pthread_mutex_unlock(&pool[i].mutex);
        }
    }

    return 0;
}