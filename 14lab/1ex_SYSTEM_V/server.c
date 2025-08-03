#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#define SHM_SIZE 1024
int main()
{
    key_t key, keysemaphore;
    int indefitier, semaphore;
    char str[] = "Hi!";
    FILE *file;
    if ((file = fopen("/tmp/shared_system_v", "w")) == NULL)
    {
        perror("Erorr: create file");
        exit(EXIT_FAILURE);
    }
    fclose(file);
    if ((key = ftok("/tmp/shared_system_v", 'A')) == -1)
    {
        perror("Failed creat key(1)");
        exit(EXIT_FAILURE);
    }
    if ((keysemaphore = ftok("/tmp/shared_system_v", 'B')) == -1)
    {
        perror("Failed creat key(2)");
        exit(EXIT_FAILURE);
    }
    if ((indefitier = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) == -1)
    {
        perror("Failed returns the identifier");
        exit(EXIT_FAILURE);
    }
    if ((semaphore = semget(keysemaphore, 2, IPC_CREAT | 0666)) == -1)
    {
        perror("Failed set identifier associated with the argument key");
        exit(EXIT_FAILURE);
    }
    char *memory = shmat(indefitier, NULL, 0);
    if (memory == (void *)-1)
    {
        perror("Failed attached shared memory segment");
        exit(EXIT_FAILURE);
    }
    semctl(semaphore, 0, SETVAL, 1);
    semctl(semaphore, 1, SETVAL, 0);
    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};
    struct sembuf wait = {1, -1, 0};
    semop(semaphore, &lock, 1);
    strncpy(memory, str, SHM_SIZE);
    semop(semaphore, &unlock, 1);
    semop(semaphore, &wait, 1);
    printf("%s\n", memory);
    if (semctl(semaphore, 0, IPC_RMID) == -1)
    {
        perror("Failed delete semaphore");
        exit(EXIT_FAILURE);
    }
    shmdt(memory);
    shmctl(indefitier, IPC_RMID, NULL);
}
