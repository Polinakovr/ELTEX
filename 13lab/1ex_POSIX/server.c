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
    char *str = "Hi!";
    char str1[10];
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 10;
    mq_unlink("/queue_m_send");
    mq_unlink("/queue_m_recv");
    mqd_t queuesend = mq_open("/queue_m_send", O_WRONLY | O_CREAT, 0666, &attr);
    if (queuesend == (mqd_t)-1)
    {
        perror("mq_open failed");
        exit(1);
    }
    if (mq_send(queuesend, str, 4, 1) == -1)
    {
        perror("Error: send message");
        exit(EXIT_FAILURE);
    }
    mq_close(queuesend);
    mqd_t queuereceive = mq_open("/queue_m_recv", O_RDONLY | O_CREAT, 0666, &attr);
     if (queuereceive == (mqd_t)-1)
    {
        perror("mq_open failed");
        exit(1);
    }
    unsigned int priority;
    if (mq_receive( queuereceive, str1, attr.mq_msgsize, &priority) == -1)
    {
        perror("Error: receive message");
        exit(EXIT_FAILURE);
    }
    printf("%s", str1);
    mq_close(queuereceive);
}