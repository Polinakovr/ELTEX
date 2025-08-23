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
    struct sockaddr_in server, client;
    int socketfd, newsocket;
    char buffer[] = "Hello!";
    char buffer1[SIZE];
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1)
    {
        printf("Error: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    if (bind(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("Error: %s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    listen(socketfd, 5);
    printf("Ожидаем установление соединения\n");
    socklen_t client_len = sizeof(client);
    newsocket = accept(socketfd, (struct sockaddr *)&client, &client_len);
    printf("Соединение произошло успешно\n");
    if (recv(newsocket, buffer1, sizeof(buffer1), 0) == -1)
    {
        printf("Error: %s", strerror(errno));
        close(newsocket);
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    if (send(newsocket, buffer, sizeof(buffer), 0) == -1)
    {
        printf("Error: %s", strerror(errno));
        close(newsocket);
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    printf("%s", buffer1);
    close(newsocket);
    close(socketfd);
}