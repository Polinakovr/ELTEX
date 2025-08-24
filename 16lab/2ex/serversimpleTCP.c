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
#define SIZE 80
int socketfd;
void signal_handler(int sig)
{
    close(socketfd);
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
    char buffer[SIZE];
    int newsockefd = *(int *)arg;
    str_time(buffer);
    if ((send(newsockefd, buffer, sizeof(buffer), 0)) == -1)
    {
        printf("%s", strerror(errno));
    }
    close(newsockefd);
    return NULL;
}
int main()
{
    struct sigaction sa;
    sa.sa_handler = &signal_handler;
    sigaction(SIGINT, &sa, NULL);
    struct sockaddr_in server, client;
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    socklen_t client_len = sizeof(client);
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    server.sin_port = htons(PORT);
    int opt = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    if (listen(socketfd, 5) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        int newsocket;
        pthread_t thread;
        newsocket = accept(socketfd, (struct sockaddr *)&client, &client_len);
        if (newsocket == -1)
        {
            if (errno = EINTR)
            {
                break;
            }
        }
        pthread_create(&thread, NULL, &handler_client,(void *) &newsocket);
        pthread_join(thread, NULL);
    }
}