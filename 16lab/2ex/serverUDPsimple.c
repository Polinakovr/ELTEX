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
int newsocketfd;
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
     struct sockaddr_in newserver;
    struct sockaddr_in client = *(struct sockaddr_in *)arg;
    if ((newsocketfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    memset(&newserver, 0, sizeof(newserver));
    newserver.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &newserver.sin_addr);
    int port = port++;
    newserver.sin_port = htons(port);
   
    str_time(buffer);
    if ((sendto(newsocketfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, sizeof(client))) == -1)
    {
        printf("%s", strerror(errno));
    }
    printf("Обработал нового клиента\n");
    return NULL;
}
int main()
{
    struct sigaction sa;
    int port = PORT;
    sa.sa_handler = &signal_handler;
    sigaction(SIGINT, &sa, NULL);
    struct sockaddr_in server, client;
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
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
    while (1)
    {
        pthread_t thread;
        char buffer[SIZE] = "\0";
        socklen_t client_len = sizeof(client);
        if (recvfrom(socketfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &client_len) == -1)
        {
            printf("%s", strerror(errno));
            close(socketfd);
            exit(EXIT_FAILURE);
        }
        printf("Увидел нового клиента\n");
        pthread_create(&thread, NULL, &handler_client, (void *)&client);
        pthread_join(thread, NULL);
        close(newsocketfd);
    }
}