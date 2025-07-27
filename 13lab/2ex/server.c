#include <fcntl.h>
#include <mqueue.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#define MAX_MESSAGE 50
#define USER_NAME 20
#define MAX_CLIENT 10
struct users
{
    char name[USER_NAME];
    char text[MAX_MESSAGE];
};
int main()
{
    struct mq_attr attr;
    unsigned count_users = 0;
    struct users *list = NULL;
    list = realloc(list, (count_users + 1) * sizeof(struct users));
    if (list == NULL)
    {
        perror("Error memory");
        exit(EXIT_FAILURE);
    }
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 50;
    char buffer[MAX_MESSAGE];
    mq_unlink("/name");
    mqd_t queue = mq_open("/name", O_CREAT | O_RDONLY, 0666, &attr);
    if (queue == (mqd_t)-1)
    {
        perror("Error create queue");
        exit(EXIT_FAILURE);
    }
    mqd_t official = mq_open("/duty", O_CREAT | O_WRONLY, 0666, &attr);
    if (official == (mqd_t)-1)
    {
        perror("Error create queue");
        exit(EXIT_FAILURE);
    }
    unsigned int prio;
    if (mq_receive(queue, buffer, MAX_MESSAGE, &prio) == -1)
    {
        perror("Error receive message");
        exit(EXIT_FAILURE);
    }
    if (prio == 6)
    {
        strncpy(list[count_users].name, buffer, USER_NAME);
        memset(buffer, 0, MAX_MESSAGE);
        if (mq_send(official,list[count_users].name,USER_NAME,5) == -1)/*отправляем новые имена*/
        {
            perror("Error send message");
            exit(EXIT_FAILURE);
        }
        count_users++;
       list= realloc(list, (count_users + 1) * sizeof(struct users));
        
    }
}