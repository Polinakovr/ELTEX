#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#define UPD_PATH "/tmp/updlocal"
#define SIZE 20
int main()
{
    struct sockaddr_un server, client;
    char buffer[SIZE];
    char buffer1[] = "Hello!";
    int socketfd;
    unlink(UPD_PATH);
    socketfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (socketfd == -1)
    {
        printf("Error 1:%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    server.sun_family = AF_LOCAL;
    strncpy(server.sun_path, UPD_PATH, sizeof(server.sun_path) - 1);
    memset(&client, 0, sizeof(client));
    socklen_t server_len = sizeof(server);
    if (bind(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("Error 5:%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    socklen_t client_len = sizeof(client);
    if (recvfrom(socketfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &client_len) == -1)
    {
        printf("Error 4:%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    if (sendto(socketfd, buffer1, sizeof(buffer1), 0, (struct sockaddr *)&client, client_len) == -1)
    {
        printf("Error 6:%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    printf("%s", buffer);
    close(socketfd);
}