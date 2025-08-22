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
    struct sockaddr_un server;
    char buffer1[] = "Hi,i'm client!";
    char buffer2[SIZE];
    int socketfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    memset(&server, 0, sizeof(server));
    server.sun_family = AF_LOCAL;
    strncpy(server.sun_path, SOCKET_PATH, sizeof(server.sun_path) - 1);
    socklen_t server_len = sizeof(server);
    int socketnew = connect(socketfd, (struct sockaddr *)&server, server_len);
    if (socketnew == -1)
    {
        printf("Ошибка :%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    if (send(socketfd, buffer1, sizeof(buffer1), 0) == -1)
    {
        printf("Ошибка :%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    if (recv(socketfd, buffer2, sizeof(buffer2), 0) == -1)
    {
        printf("Ошибка :%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    printf("%s \n", buffer2);
    close(socketfd);
    return 0;
}