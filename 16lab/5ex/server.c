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
#define SIZE 20
int main()
{
    char buffer[SIZE];
    char buffer1[] = "Hello";
    int socketfd;
    struct sockaddr_in server, client;
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    server.sin_port = htons(PORT);
    socklen_t client_len = sizeof(client);
    bind(socketfd, (struct sockaddr *)&server, sizeof(server));

    if (recvfrom(socketfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &client_len) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    if (sendto(socketfd, buffer1, sizeof(buffer1), 0, (struct sockaddr *)&client, client_len) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    printf("%s \n", buffer);
    close(socketfd);
    return 0;
}