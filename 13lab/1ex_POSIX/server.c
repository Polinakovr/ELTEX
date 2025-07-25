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
 struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = 1024,  
        .mq_curmsgs = 0
    }
int main()
{
    char *str = "Hi!";
    char str1[10];
    mqd_t queue = mq_open("/queue_m", O_RDWR | O_CREAT, 0666, &attr); /*Почитать про параметры*/
    if (queue == (mqd_t)-1)
    {
        perror("mq_open failed");
        exit(1);
    }
    if (mq_send(queue, str, 4, 1) == -1)
    {
        perror("Error: send message");
        exit(EXIT_FAILURE);
    }
    unsigned int priority = 2;
    if (mq_receive(queue, str1, 10, &priority) == -1)
    {
        perror("Error: receive message");
        exit(EXIT_FAILURE);
    } /*удаление добавить а так же решить ошибку с размером сообщения */
}