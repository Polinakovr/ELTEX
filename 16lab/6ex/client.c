#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#define PORT 8081
#define PORT_SRC 8080
#define SIZE 1024
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
    int opt = 1;
    if ((setsockopt(socketfd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt)) == -1))
    {
        printf("IP error:%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    char packets[sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(buffer1)];
    struct iphdr *headerip = (struct iphdr *)packets;
    headerip->version = 4;
    headerip->ihl = 5;
    headerip->tos = 0;
    headerip->tot_len = htons((sizeof(struct iphdr)) + sizeof(struct udphdr) + sizeof(buffer1));
    headerip->frag_off = 0;
    headerip->ttl = 64;
    headerip->protocol = IPPROTO_UDP;
    headerip->check = 0;
    inet_pton(AF_INET, "127.0.0.1", &headerip->saddr);
    inet_pton(AF_INET, "127.0.0.1", &headerip->daddr);
    struct udphdr *header = (struct udphdr *)(packets + sizeof(struct iphdr));
    header->source = htons(PORT_SRC);
    header->dest = htons(PORT);
    header->len = htons(sizeof(struct udphdr) + sizeof(buffer1));
    header->check = 0;

    memcpy(packets + sizeof(struct udphdr) + sizeof(struct iphdr), buffer1, sizeof(buffer1));
    char packetsrecv[sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(buffer)];
    if ((sendto(socketfd, packets, sizeof(packets), 0, (struct sockaddr *)&server, server_len)) == -1)
    {
        printf("%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        int recvsize;
        recvsize = recvfrom(socketfd, packetsrecv, sizeof(packetsrecv), 0, (struct sockaddr *)&server, &server_len);
        if (recvsize == -1)
        {
            printf("%s", strerror(errno));
            close(socketfd);
            exit(EXIT_FAILURE);
        }

        struct udphdr *headrerec = (struct udphdr *)(packetsrecv + sizeof(struct iphdr));
        if (ntohs(headrerec->dest) == PORT_SRC)
        {
          char *data = (char *)(packetsrecv + sizeof(struct iphdr) + sizeof(struct udphdr));
            int data_length =recvsize - sizeof(struct iphdr) - sizeof(struct udphdr);
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