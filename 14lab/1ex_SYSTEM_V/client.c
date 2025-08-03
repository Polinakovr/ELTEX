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
    char str[] = "Hello!";
    if ((key = ftok("/tmp/shared_system_v", 'A')) == -1)
    {
        perror("Failed creat key(3)");
        exit(EXIT_FAILURE);
    }
    if ((keysemaphore = ftok("/tmp/shared_system_v", 'B')) == -1)
    {
        perror("Failed creat key(4)");
        exit(EXIT_FAILURE);
    }
    if ((indefitier = shmget(key, SHM_SIZE, 0666)) == -1)
    {
        perror("Failed returns the identifier");
        exit(EXIT_FAILURE);
    }
    if ((semaphore = semget(keysemaphore, 2, 0666)) == -1)
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
  
    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};
    struct sembuf nowait = {1, 1, 0};
    semop(semaphore, &lock, 1);
    printf("%s\n", memory); 
    strncpy(memory, str, SHM_SIZE);
    semop(semaphore, &unlock, 1);
    semop(semaphore, &nowait, 1);
    shmdt(memory);
}