#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8081
#define SIZE 20
int main()
{
      struct sockaddr_in  server;
    int socketfd;
    char buffer[] = "Hi server!";
    char buffer1[SIZE];
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd == -1)
    {
        printf("Error:%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
   
    socklen_t server_len = sizeof(server); 
    if (sendto(socketfd, buffer,sizeof(buffer), 0, (struct sockaddr *)&server,server_len) == -1)
    {
        printf("Error:%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    if (recvfrom(socketfd, buffer1, sizeof(buffer1), 0, (struct sockaddr *)&server, &server_len) == -1)
    {
        printf("Error:%s", strerror(errno));
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    printf("%s",buffer1);
}