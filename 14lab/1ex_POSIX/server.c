#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#define SIZE_BLOCK 128
int main()
{
    int shared_memory_object;
    char str[] = "Hi!";
     if ((shared_memory_object = shm_open("/posix_shared_memory", O_CREAT | O_RDWR, 0666)) == -1)
    {
        perror("Error create shared memory");
        exit(EXIT_FAILURE);
    }
    ftruncate(shared_memory_object, SIZE_BLOCK);
    char *memory = mmap(NULL, SIZE_BLOCK, PROT_WRITE | PROT_READ, MAP_SHARED, shared_memory_object, 0);
    sem_t *sem1 = sem_open("/sem_block", O_CREAT, 0666, 1);
    sem_t *sem2 = sem_open("/sem_cond", O_CREAT, 0666, 0);
    sem_wait(sem1);
    strncpy(memory, str, SIZE_BLOCK);
    sem_post(sem1);
    sem_wait(sem2);
    printf("%s",memory);
    munmap(memory, SIZE_BLOCK);
    shm_unlink("/posix_shared_memory");
}