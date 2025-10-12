#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
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
#define SIZE_RECV 1024

int socketfd;
struct sockaddr_in server;
struct sockaddr_in client;
socklen_t server_len;

void send_close_message()
{
    char close_msg[] = "close";
    char packets[sizeof(struct udphdr) + sizeof(close_msg)];
    struct udphdr *header = (struct udphdr *)packets;

    header->source = htons(client.sin_port);
    header->dest = htons(PORT);
    header->len = htons(sizeof(struct udphdr) + sizeof(close_msg));
    header->check = 0;

    memcpy(packets + sizeof(struct udphdr), close_msg, sizeof(close_msg));

    if (sendto(socketfd, packets, sizeof(packets), 0, (struct sockaddr *)&server, server_len) == -1)
    {
        printf("Ошибка отправки close: %s\n", strerror(errno));
    }
    else
    {
        printf("Отправлено сообщение 'close' серверу\n");
    }
}

void signal_handler(int sig)
{
    send_close_message();
    close(socketfd);
    exit(EXIT_SUCCESS);
}

int main()
{
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    srand(time(NULL));

    if ((socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1)
    {
        printf("socket error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    server.sin_port = htons(PORT);
    server_len = sizeof(server);

    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = INADDR_ANY;
    client.sin_port = htons(10000 + (rand() % 1000));

    if (bind(socketfd, (struct sockaddr *)&client, sizeof(client)) == -1)
    {
        printf("bind error: %s\n", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    printf("Клиент запущен на порту %d\n", ntohs(client.sin_port));

    while (1)
    {
        char buffer[SIZE];
        char recvbuffer[SIZE_RECV];

        if (fgets(buffer, SIZE, stdin) == NULL)
        {
            printf("fgets error: %s\n", strerror(errno));
            close(socketfd);
            exit(EXIT_FAILURE);
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        char packets[sizeof(struct udphdr) + strlen(buffer) + 1];
        struct udphdr *header = (struct udphdr *)packets;

        header->source = htons(client.sin_port);
        header->dest = htons(PORT);
        header->len = htons(sizeof(struct udphdr) + strlen(buffer) + 1);
        header->check = 0;

        memcpy(packets + sizeof(struct udphdr), buffer, strlen(buffer) + 1);

        printf("Отправляем: %s\n", buffer);

        if (sendto(socketfd, packets, sizeof(packets), 0, (struct sockaddr *)&server, server_len) == -1)
        {
            printf("sendto error: %s\n", strerror(errno));
            close(socketfd);
            exit(EXIT_FAILURE);
        }

        while (1)
        {
            int recvsize = recvfrom(socketfd, recvbuffer, SIZE_RECV, 0, NULL, NULL);
            if (recvsize == -1)
            {
                printf("recvfrom error: %s\n", strerror(errno));
                continue;
            }

            struct iphdr *iph = (struct iphdr *)recvbuffer;
            int ip_header_len = iph->ihl * 4;

            struct udphdr *udph = (struct udphdr *)(recvbuffer + ip_header_len);

            if (ntohs(udph->dest) == client.sin_port && ntohs(udph->source) == PORT)
            {
                char *data = (char *)(udph + 1);
                int data_length = recvsize - ip_header_len - sizeof(struct udphdr);

                if (data_length > 0)
                {
                    char msg[SIZE_RECV];
                    memcpy(msg, data, data_length);
                    msg[data_length] = '\0';
                    printf("Получено от сервера: %s\n", msg);
                    break;
                }
            }
        }
    }

    close(socketfd);
    return 0;
}