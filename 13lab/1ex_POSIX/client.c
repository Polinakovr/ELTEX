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
    mqd_t queuerecv = mq_open("/queue_m_send", O_RDONLY); /*Почитать про параметры*/
    if (queuerecv == (mqd_t)-1)
    {
        perror("Error: entry queue");
        exit(EXIT_FAILURE);
    }

    unsigned int priority;
    if (mq_receive(queuerecv, str1, 10, &priority) == -1)
    {
        perror("Error: receive message");
        exit(EXIT_FAILURE);
    }
    printf("%s", str1);
    mqd_t queuesend = mq_open("/queue_m_recv", O_WRONLY);
    if (queuesend == (mqd_t)-1)
    {
        perror("Error: entry queue");
        exit(EXIT_FAILURE);
    }
    if (mq_send(queuesend, str, 7, 2) == -1)
    {
        perror("Error: send message");
        exit(EXIT_FAILURE);
    }
}