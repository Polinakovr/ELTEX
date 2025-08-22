#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#define UPD_PATH "/tmp/updlocal"
#define UPD_PATH_CLIENT "/tmp/updlocalclient"
#define SIZE 20
int main()
{
    struct sockaddr_un server, client;
    char buffer[SIZE];
    char buffer1[] = "Hi!";
    int socketfd;
    socketfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (socketfd == -1)
    {
        printf("Error:%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    server.sun_family = AF_LOCAL;
    strncpy(server.sun_path, UPD_PATH, sizeof(server.sun_path));
    socklen_t server_len = sizeof(server);
    client.sun_family = AF_LOCAL;
    strncpy(client.sun_path, UPD_PATH_CLIENT, sizeof(client.sun_path));
    if (bind(socketfd, (struct sockaddr *)&client, sizeof(client)) == -1)
    {
        printf("Ошибка 2:%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    if (connect(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("Error:%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    if (send(socketfd, buffer1, sizeof(buffer1), 0) == -1)
    {
        printf("Error:%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    if (recv(socketfd, buffer, sizeof(buffer), 0) == -1)
    {
        printf("Error:%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    printf("%s", buffer);
    close(socketfd);
}