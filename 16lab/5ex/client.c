#include <arpa/inet.h>
#include <errno.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#define PORT 8081
#define IP_HEADER_LENGTH 20
#define PORT_SRC 8080
#define SIZE 80
int main()
{
    char buffer[SIZE];
    char buffer1[] = "Hi";
    int socketfd;
    struct sockaddr_in server, client;
    if ((socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1)
    {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    server.sin_port = htons(PORT);
    socklen_t server_len = sizeof(server);

    char packets[sizeof(struct udphdr) + sizeof(buffer1)];
    struct udphdr *header = (struct udphdr *)packets;
    header->source = htons(PORT_SRC);
    header->dest = htons(PORT);
    header->len = htons(sizeof(struct udphdr) + sizeof(buffer1));
    header->check = 0;
    memcpy(packets + sizeof(struct udphdr), buffer1, sizeof(buffer1));
    char packetsrecv[sizeof(struct udphdr) + sizeof(buffer) + (sizeof(char) * IP_HEADER_LENGTH)];
    if ((sendto(socketfd, packets, sizeof(struct udphdr) + sizeof(buffer1), 0, (struct sockaddr *)&server, server_len))
        == -1)
    {
        printf("%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    while (1) 
    {
        int recvsize;
        recvsize =recvfrom(socketfd, packetsrecv, sizeof(struct udphdr) + sizeof(buffer) + sizeof(char) * IP_HEADER_LENGTH, 0,
                     (struct sockaddr *)&server, &server_len);
        if (recvsize == -1)
        {
            printf("%s", strerror(errno));
            close(socketfd);
            exit(EXIT_FAILURE);
        }

        struct udphdr *headrerec = (struct udphdr *)(packetsrecv + (sizeof(char) * IP_HEADER_LENGTH));
        if (ntohs(headrerec->dest) == PORT_SRC)
        {
            char *data = (char *)(headrerec + 1);
            int data_length = recvsize - IP_HEADER_LENGTH - sizeof(struct udphdr);
            char msg[SIZE];
            memcpy(msg, data, data_length);
            printf("%s \n", msg);
            break;
        }
        else
        {
            continue;
        }
    }
    close(socketfd);
    return 0;
}