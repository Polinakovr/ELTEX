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
    char str[] = "Hello!" ;
    if ((shared_memory_object = shm_open("/posix_shared_memory", O_RDWR, 0666)) == -1)
    {
        perror("Error create shared memory");
        exit(EXIT_FAILURE);
    }
    char *memory = mmap(NULL, SIZE_BLOCK, PROT_WRITE | PROT_READ, MAP_SHARED, shared_memory_object, 0);
    sem_t *sem1 = sem_open("/sem_block", 0);
    sem_t *sem2 = sem_open("/sem_cond", 0);
    sem_wait(sem1);
    printf("%s",memory);
    strncpy(memory, str, SIZE_BLOCK);
    sem_post(sem1);
    sem_post(sem2);
    
    
}