#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
struct msgbuf
{
    long mtype;
    char mtext[20];
};
int main()
{
    key_t key;
    int queue;
    if ((key = ftok("/tmp/keysystemsend", 'a')) == -1)
    {
        perror("Erorr: create key");
        exit(EXIT_FAILURE);
    }

    if ((queue = msgget(key, 0666))==-1)
    {
        perror("Erorr: create queue message");
        exit(EXIT_FAILURE);
    }

    struct msgbuf m_server;
    struct msgbuf m_client;
    m_client.mtype = 2;
    snprintf(m_client.mtext, sizeof(m_client.mtext), "Hello!");

    if (msgrcv(queue, &m_server, sizeof(m_server.mtext), 1, 0) == -1)
    {
        perror("Erorr: recv message");
        exit(EXIT_FAILURE);
    }
    printf("%s", m_server.mtext);
    if (msgsnd(queue, &m_client, sizeof(m_client.mtext), 0) == -1)
    {
        perror("Erorr: send message");
        exit(EXIT_FAILURE);
    }
}