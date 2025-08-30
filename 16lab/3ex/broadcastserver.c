#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#define PORT 8080
int main()
{
    struct sockaddr_in client;
    int socketfd;
    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);
    char buffer[] = "Hello!";
     inet_pton(AF_INET, "255.255.255.255", &client.sin_addr);
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    int op = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_BROADCAST, &op, sizeof(op)) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    
    if (sendto(socketfd, buffer, sizeof(buffer),0, (struct sockaddr *)&client, sizeof(client)) == -1)
    {
        if (EINTR == errno)
        {
            printf("Ошибочный сигнал");
        }
        printf("%s", strerror(errno));
        close(socketfd);
    }
    close(socketfd);
    return 0;
}