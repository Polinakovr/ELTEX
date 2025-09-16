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
#define PORT 8081
#define SIZE 20
int main()
{
    struct sockaddr_in client;
    int socketfd;
    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);
    char buffer[SIZE];
    client.sin_addr.s_addr=htonl(INADDR_ANY);
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (bind(socketfd, (struct sockaddr *)&client, sizeof(client)) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    struct ip_mreq mreq;
    inet_pton(AF_INET, "224.0.0.1", &mreq.imr_multiaddr.s_addr);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(socketfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    socklen_t client_len = sizeof(client);
    if (recvfrom(socketfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &client_len) == -1)
    {
        if (EINTR == errno)
        {
            printf("Ошибочный сигнал");
        }
        printf("%s", strerror(errno));
        close(socketfd);
    }
    printf("%s", buffer);
    close(socketfd);
    return 0;
}