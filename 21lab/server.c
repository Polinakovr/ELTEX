#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PORT 8081
#define IP_HEADER_LENGTH 20
#define SIZE 20
#define CLIENT 10

int socketfd;

typedef struct
{
    struct sockaddr_in client;
    int count;
    bool active;
    char buffer[SIZE];
    pthread_mutex_t mutex;
} clients;

clients list[CLIENT];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int find_client(struct sockaddr_in *client_addr, in_port_t client_port)
{
    for (int i = 0; i < CLIENT; i++)
    {
        if (list[i].active &&
            list[i].client.sin_addr.s_addr == client_addr->sin_addr.s_addr &&
            list[i].client.sin_port == client_port)
        {
            return i;
        }
    }
    return -1;
}

int find_free_slot()
{
    for (int i = 0; i < CLIENT; i++)
    {
        if (!list[i].active)
        {
            return i;
        }
    }
    return -1;
}

void signal_handler(int sig)
{
    close(socketfd);
    for (int i = 0; i < CLIENT; i++)
    {
        pthread_mutex_destroy(&list[i].mutex);
    }
    exit(EXIT_SUCCESS);
}

void *handler_client(void *arg)
{
    clients *clientnew = (clients *)arg;

    pthread_mutex_lock(&clientnew->mutex);
    struct sockaddr_in client_addr = clientnew->client;
    socklen_t client_size = sizeof(client_addr);
    int count = clientnew->count;
    char message[SIZE];
    strncpy(message, clientnew->buffer, SIZE);
    pthread_mutex_unlock(&clientnew->mutex);

    int needed_size = snprintf(NULL, 0, "%s %d", message, count) + 1;
    char buffer[needed_size];
    snprintf(buffer, needed_size, "%s %d", message, count);

    char packets[sizeof(struct udphdr) + needed_size];
    struct udphdr *header = (struct udphdr *)packets;
    header->source = htons(PORT);
    header->dest = client_addr.sin_port;
    header->len = htons(sizeof(struct udphdr) + needed_size);
    header->check = 0;
    memcpy(packets + sizeof(struct udphdr), buffer, needed_size);

    if (sendto(socketfd, packets, sizeof(packets), 0, (struct sockaddr *)&client_addr, client_size) == -1)
    {
        printf("sendto error: %s\n", strerror(errno));
    }
    else
    {
        printf("Отправили клиенту %s:%d: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
    }

    pthread_mutex_lock(&clientnew->mutex);
    if (strncmp(message, "close", 5) == 0)
    {
        clientnew->active = false;
        clientnew->count = 0;
        memset(&clientnew->client, 0, sizeof(clientnew->client));
        memset(clientnew->buffer, 0, SIZE);
        printf("Клиент %s:%d закрыл соединение\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }
    pthread_mutex_unlock(&clientnew->mutex);

    printf("Завершили обработку\n");
    return NULL;
}

int main()
{
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);
sigaction(SIGTERM, &sa, NULL);
    if ((socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1)
    {
        printf("socket error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < CLIENT; i++)
    {
        list[i].active = false;
        list[i].count = 0;
        pthread_mutex_init(&list[i].mutex, NULL);
    }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("bind error: %s\n", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на порту %d\n", PORT);

    while (1)
    {
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);
        char packetsrecv[sizeof(struct udphdr) + SIZE + IP_HEADER_LENGTH];

        int recvsize = recvfrom(socketfd, packetsrecv, sizeof(packetsrecv), 0, (struct sockaddr *)&client, &client_len);
        if (recvsize == -1)
        {
            printf("recvfrom error: %s\n", strerror(errno));
            continue;
        }

        struct iphdr *iph = (struct iphdr *)packetsrecv;
        int ip_header_len = iph->ihl * 4;
        struct udphdr *headrerec = (struct udphdr *)(packetsrecv + ip_header_len);
        if (ntohs(headrerec->dest) != PORT)
        {
            continue;
        }

        client.sin_port = headrerec->source;

        char *data = (char *)(headrerec + 1);
        int data_length = recvsize - ip_header_len - sizeof(struct udphdr);
        char message[SIZE];
        memcpy(message, data, data_length);
        message[data_length] = '\0';
        printf("Получили данные от %s:%d: %s\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), message);

        pthread_mutex_lock(&clients_mutex);
        int client_index = find_client(&client, client.sin_port);
        if (client_index == -1)
        {
            client_index = find_free_slot();
            if (client_index == -1)
            {
                printf("Нет свободных слотов для клиента\n");
                pthread_mutex_unlock(&clients_mutex);
                continue;
            }
        }

        pthread_mutex_lock(&list[client_index].mutex);
        memcpy(&list[client_index].client, &client, client_len);
        memcpy(list[client_index].buffer, message, data_length + 1);
        list[client_index].count++;
        list[client_index].active = true;
        pthread_mutex_unlock(&list[client_index].mutex);

        pthread_t thread;
        if (pthread_create(&thread, NULL, handler_client, (void *)&list[client_index]) != 0)
        {
            printf("Ошибка создания потока: %s\n", strerror(errno));
            pthread_mutex_unlock(&clients_mutex);
            continue;
        }
        pthread_detach(thread);
        pthread_mutex_unlock(&clients_mutex);
    }

    return 0;
}