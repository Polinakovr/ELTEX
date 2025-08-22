#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#define SIZE 20
#define SOCKET_PATH "/tmp/socket"
int main()
{
    unlink(SOCKET_PATH);
    struct sockaddr_un client, server;
    char buffer1[] = "Hello!";
    char buffer2[SIZE];
    int socketfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (socketfd == -1)
    {
        printf("Ошибка: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    memset(&server, 0, sizeof(server));
    server.sun_family = AF_LOCAL;
    strncpy(server.sun_path, SOCKET_PATH, sizeof(server.sun_path) - 1);
    socklen_t client_len = sizeof(client);
    if (bind(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("Ошибка 2:%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    if (listen(socketfd, 5) == -1)
    {
        printf("Ошибка:%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    printf("Сервер ожидает подключения...\n");
    int socketconnect = accept(socketfd, (struct sockaddr *)&client, &client_len);
    if (socketconnect == -1)
    {
        printf("Ошибка:%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    printf("Клиент подключился к серверу...\n");
    if (recv(socketconnect, buffer2, SIZE, 0) == -1)
    {
        printf("Ошибка:%s", strerror(errno));
        close(socketfd);
        close(socketconnect);
        exit(EXIT_FAILURE);
    }
    if (send(socketconnect, buffer1, sizeof(buffer1), 0) == -1)
    {
        printf("Ошибка:%s", strerror(errno));
        close(socketfd);
        close(socketconnect);
        exit(EXIT_FAILURE);
    }
    printf("%s \n", buffer2);
}