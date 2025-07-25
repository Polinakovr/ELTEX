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
    FILE *file;
    if ((file = fopen("/tmp/keysystemsend", "w")) == NULL)
    {
        perror("Erorr: create file");
        exit(EXIT_FAILURE);
    }

    fclose(file);
    key_t key;
    int queue;
    if ((key = ftok("/tmp/keysystemsend", 'a') )== -1)
    {
        perror("Erorr: create key");
        exit(EXIT_FAILURE);
    }

    if ((queue = msgget(key, 0666 | IPC_CREAT))==-1)
    {
        perror("Erorr: create queue message");
        exit(EXIT_FAILURE);
    }

    struct msgbuf m_server;
    struct msgbuf m_client;
    m_server.mtype = 1;
    snprintf(m_server.mtext, sizeof(m_server.mtext), "Hi!");
    if (msgsnd(queue, &m_server, sizeof(m_server.mtext), 0) == -1)
    {
        perror("Erorr: send message");
        exit(EXIT_FAILURE);
    }
    if (msgrcv(queue, &m_client, sizeof(m_client.mtext), 2, 0) == -1)
    {
        perror("Erorr: recv message");
        exit(EXIT_FAILURE);
    }
    printf("%s", m_client.mtext);
    if (msgctl(queue, IPC_RMID, 0) == -1)
    {
        perror("Erorr: delete queue");
        exit(EXIT_FAILURE);
    }
}