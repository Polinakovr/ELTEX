#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define PORT 8080
#define SIZE 20
int main()
{
    struct sockaddr_in server;
    int socketfd;
    char buffer[] = "Hi!";
    char buffer1[SIZE];
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Error: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    if (connect(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("Error: %s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    if (send(socketfd, buffer, sizeof(buffer), 0) == -1)
    {
        printf("Error: %s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    if (recv(socketfd, buffer1, sizeof(buffer1), 0) == -1)
    {
        printf("Error: %s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    printf("%s", buffer1);
    close(socketfd);
}