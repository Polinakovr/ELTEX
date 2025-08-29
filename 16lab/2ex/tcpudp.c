#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#define PORT_TCP 8080
#define PORT_UDP 8081
#define PORT 8080
#define SIZE 80
#define MAX_EVENTS 10
int socketfdtcp;
int socketfdudp;
int port;
void signal_handler(int sig)
{
    close(socketfdtcp);
    close(socketfdudp);
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
void handler_client_udp()
{
    struct sockaddr_in client, newserver;
    char buffer[SIZE] = "\0";
    int newsocketfd;
    socklen_t client_len = sizeof(client);
    if (recvfrom(socketfdudp, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &client_len) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfdtcp);
        close(socketfdudp);
        exit(EXIT_FAILURE);
    }
    printf("Увидел клиента %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    if ((newsocketfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfdtcp);
        close(socketfdudp);
        exit(EXIT_FAILURE);
    }
    memset(&newserver, 0, sizeof(newserver));
    newserver.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &newserver.sin_addr);
    port++;
    newserver.sin_port = htons(port);
    str_time(buffer);
    if ((sendto(newsocketfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, sizeof(client))) == -1)
    {
        printf("%s", strerror(errno));
    }
    printf("Обработал нового клиента\n");
    close(newsocketfd);
}
void handler_client_tcp()
{
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int newsocket = accept(socketfdtcp, (struct sockaddr *)&client, &client_len);
    printf("Увидел клиента %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    char buffer[SIZE];
    str_time(buffer);
    if ((send(newsocket, buffer, sizeof(buffer), 0)) == -1)
    {
        printf("%s", strerror(errno));
    }
    close(newsocket);
}
int main()
{
    struct sigaction sa;
    sa.sa_handler = &signal_handler;
    sigaction(SIGINT, &sa, NULL);
    struct sockaddr_in servertcp;
    struct sockaddr_in serverudp;
    port = PORT_UDP;
    if ((socketfdtcp = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if ((socketfdudp = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    memset(&servertcp, 0, sizeof(servertcp));
    servertcp.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servertcp.sin_addr);
    servertcp.sin_port = htons(PORT_TCP);
    memset(&serverudp, 0, sizeof(serverudp));
    serverudp.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serverudp.sin_addr);
    serverudp.sin_port = htons(PORT_UDP);
    int opt = 1;
    setsockopt(socketfdtcp, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(socketfdudp, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(socketfdtcp, (struct sockaddr *)&servertcp, sizeof(servertcp)) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfdtcp);
        close(socketfdudp);
        exit(EXIT_FAILURE);
    }
    if (bind(socketfdudp, (struct sockaddr *)&serverudp, sizeof(serverudp)) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfdtcp);
        close(socketfdudp);
        exit(EXIT_FAILURE);
    }
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = socketfdtcp;
    struct epoll_event ev1;
    ev1.events = EPOLLIN;
    ev1.data.fd = socketfdudp;
    int fd1 = epoll_create1(0);
    if (fd1 == -1)
    {
        printf("%s", strerror(errno));
        close(socketfdtcp);
        close(socketfdudp);
        exit(EXIT_FAILURE);
    }
    if ((epoll_ctl(fd1, EPOLL_CTL_ADD, socketfdtcp, &ev)) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfdtcp);
        close(socketfdudp);
        close(fd1);
        exit(EXIT_FAILURE);
    }
    if ((epoll_ctl(fd1, EPOLL_CTL_ADD, socketfdudp, &ev1)) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfdtcp);
        close(socketfdudp);
        close(fd1);
        exit(EXIT_FAILURE);
    }
    if (listen(socketfdtcp, 5) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfdtcp);
        close(socketfdudp);
        close(fd1);
        exit(EXIT_FAILURE);
    }
    struct epoll_event events[10];
    int num_events;
    while (1)
    {
        if ((num_events = epoll_wait(fd1, events, MAX_EVENTS, -1)) == -1)
        {
            if (errno == EINTR)
                continue;
            break;
        }
        for (int i = 0; i < num_events; i++)
        {
            if (events[i].data.fd == socketfdtcp)
            {
                handler_client_tcp();
            }
            if (events[i].data.fd == socketfdudp)
            {
                handler_client_udp();
            }
        }
    }
}