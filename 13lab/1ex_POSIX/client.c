#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
int main()
{
    char *str = "Hello!";
    char str1[10];
    mqd_t queue = mq_open("queue_m", O_RDWR); /*Почитать про параметры*/
    if (queue == -1)
    {
        perror("Error: entry queue");
        exit(EXIT_FAILURE);
    } unsigned int priority=1;
     if (mq_receive(queue, str1, 10, &priority) == -1)
    {
        perror("Error: receive message");
        exit(EXIT_FAILURE);
    }if (mq_send(queue, str, 7, 2) == -1)
    {
        perror("Error: send message");
        exit(EXIT_FAILURE);
    }
}