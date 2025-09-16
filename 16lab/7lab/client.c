#include <arpa/inet.h>
#include <errno.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
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
#define ETH_HEADER 14
#define PORT_SRC 8080
#define SIZE 1024
unsigned short checksum(struct iphdr *ip)
{
    unsigned short *buf = (unsigned short *)ip;
    unsigned int sum = 0;
    for (int i = 0; i < 10; i++)
    {
        sum = sum + buf[i];
        if (sum > 0xFFFF)
        {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
    }
    return ~sum;
}
int main()
{
    char buffer[SIZE];
    char buffer1[] = "Hi";
    int socketfd;
    struct sockaddr_ll server, client;

    if ((socketfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1)
    {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    memset(&server, 0, sizeof(server));
    server.sll_family = AF_PACKET;
    server.sll_ifindex = if_nametoindex("enp0s8");
    server.sll_halen = 6;
    server.sll_addr[0] = 0x08;
    server.sll_addr[1] = 0x00;
    server.sll_addr[2] = 0x27;
    server.sll_addr[3] = 0xe9;
    server.sll_addr[4] = 0x13;
    server.sll_addr[5] = 0xea;
    server.sll_addr[6] = 0x00;
    server.sll_addr[7] = 0x00;
    socklen_t server_len = sizeof(server);
    char packets[ETH_HEADER + sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(buffer1)];
    unsigned int dst[6] = {0x08, 0x00, 0x27, 0xe9, 0x13, 0xea};
    unsigned int src[6] = {0x08, 0x00, 0x27, 0xd3, 0x81, 0x7f};
    memcpy(packets, dst, 6);
    memcpy(packets + 6, src, 6);
    unsigned short ethertype = htons(0x0800);
    memcpy(packets + 12, &ethertype, 2);
    struct iphdr *headerip = (struct iphdr *)(packets + ETH_HEADER);
    headerip->version = 4;
    headerip->ihl = 5;
    headerip->tos = 0;
    headerip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(buffer1));
    headerip->frag_off = 0;
    headerip->ttl = 64;
    headerip->id = htons(1234);
    headerip->protocol = IPPROTO_UDP;
    inet_pton(AF_INET, "192.168.56.102", &headerip->saddr);
    inet_pton(AF_INET, "192.168.56.101", &headerip->daddr);
    headerip->check = checksum(headerip);

   struct udphdr *header = (struct udphdr *)(packets + ETH_HEADER + sizeof(struct iphdr));
    header->source = htons(PORT_SRC);
    header->dest = htons(PORT);
    header->len = htons(sizeof(struct udphdr) + sizeof(buffer1));
    header->check = 0;

   memcpy(packets + ETH_HEADER + sizeof(struct iphdr) + sizeof(struct udphdr), buffer1, sizeof(buffer1));
    char packetsrecv[ETH_HEADER + sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(buffer)];
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

        struct udphdr *headrerec = (struct udphdr *)(packetsrecv + ETH_HEADER + sizeof(struct iphdr));
        if (ntohs(headrerec->dest) == PORT_SRC)
        {
            char *data = (char *)(packetsrecv + ETH_HEADER  +sizeof(struct iphdr) + sizeof(struct udphdr));
            int data_length = recvsize - ETH_HEADER - sizeof(struct iphdr) - sizeof(struct udphdr);
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