#include "driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <poll.h>
#include <pthread.h>

typedef struct
{
    pid_t pid;
    bool available;
    bool busy;
    int to_driver;
    int from_driver;
    int time_remaining;
    pthread_t timer_thread;
    pthread_mutex_t mutex;
} driver_info;

driver_info *drivers_list = NULL;
int active_drivers = 0;

void init()
{
    drivers_list = (driver_info *)malloc(NUM_DRIVERS * sizeof(driver_info));
    if (drivers_list == NULL)
    {
        printf("%s\n", strerror(errno));
        return;
    }
    for (int i = 0; i < NUM_DRIVERS; i++)
    {
        drivers_list[i].available = true;
        drivers_list[i].busy = false;
        drivers_list[i].time_remaining = 0;
        pthread_mutex_init(&drivers_list[i].mutex, NULL);
    }
    active_drivers = 0;
}

void* timer_thread(void* arg)
{
    int driver_index = *(int*)arg;
    free(arg);
    
    while (1)
    {
        sleep(1);
        
        pthread_mutex_lock(&drivers_list[driver_index].mutex);
        
        if (drivers_list[driver_index].busy && drivers_list[driver_index].time_remaining > 0)
        {
            drivers_list[driver_index].time_remaining--;
            
            if (drivers_list[driver_index].time_remaining == 0)
            {
                drivers_list[driver_index].busy = false;
                printf("Водитель %d: задача завершена\n", drivers_list[driver_index].pid);
                pthread_mutex_unlock(&drivers_list[driver_index].mutex);
                break;
            }
        }
        else if (!drivers_list[driver_index].busy)
        {
            pthread_mutex_unlock(&drivers_list[driver_index].mutex);
            break;
        }
        
        pthread_mutex_unlock(&drivers_list[driver_index].mutex);
    }
    
    return NULL;
}

pid_t create_driver()
{
    if (active_drivers >= NUM_DRIVERS)
    {
        printf("Достигнуто максимальное количество водителей\n");
        return -1;
    }

    pid_t pid;
    int to_driver[2];
    int from_driver[2];
    
    if (pipe(to_driver) == -1 || pipe(from_driver) == -1)
    {
        printf("%s\n", strerror(errno));
        return -1;
    }
    
    switch (pid = fork())
    {
        case -1: 
            printf("%s\n", strerror(errno));
            return -1;
        case 0:
            close(to_driver[1]);
            close(from_driver[0]);
            task(to_driver[0], from_driver[1]);
            exit(EXIT_SUCCESS);
        default:
            close(to_driver[0]);
            close(from_driver[1]);
            
            for (int i = 0; i < NUM_DRIVERS; i++)
            {
                if (drivers_list[i].available == true)
                {
                    drivers_list[i].pid = pid;
                    drivers_list[i].available = false;
                    drivers_list[i].busy = false;
                    drivers_list[i].time_remaining = 0;
                    drivers_list[i].to_driver = to_driver[1];
                    drivers_list[i].from_driver = from_driver[0];
                    active_drivers++;
                    printf("Создан водитель с PID: %d\n", pid);
                    return pid;
                }
            }
            return -1;
    }
}

void send_task(int pid, int seconds)
{
    for (int i = 0; i < NUM_DRIVERS; i++)
    {
        if (drivers_list[i].pid == pid)
        {
            pthread_mutex_lock(&drivers_list[i].mutex);
            
            if (drivers_list[i].busy == false)
            {
                char command[20];
                sprintf(command, "TASK %d", seconds);
                write(drivers_list[i].to_driver, command, strlen(command) + 1);
                
                char response[20];
                read(drivers_list[i].from_driver, response, sizeof(response));

                if (strcmp(response, "OK") == 0)
                {
                    drivers_list[i].busy = true;
                    drivers_list[i].time_remaining = seconds;
                    
                   
                    int *driver_index = malloc(sizeof(int));
                    *driver_index = i;
                    pthread_create(&drivers_list[i].timer_thread, NULL, timer_thread, driver_index);
                    pthread_detach(drivers_list[i].timer_thread);
                    
                    printf("Водитель %d принял задачу на %d секунд\n", pid, seconds);
                }
            }
            else
            {
                write(drivers_list[i].to_driver, "STATUS", 7);
                char response[50];
                read(drivers_list[i].from_driver, response, sizeof(response));
                if (strncmp(response, "Busy", 4) == 0)
                {
                    int time_remaining;
                    sscanf(response, "Busy %d", &time_remaining);
                    printf("Водитель %d занят. Осталось времени: %d секунд\n", pid, time_remaining);
                }
            }
            
            pthread_mutex_unlock(&drivers_list[i].mutex);
            break;
        }
    }
}

void task(int to_driver, int from_driver)
{
    int time_remaining = 0;
    char buffer[50];
    
    while (1)
    {
      
        struct pollfd fds[1];
        fds[0].fd = to_driver;
        fds[0].events = POLLIN;
        
        int ret = poll(fds, 1, 100);
        
        if (ret > 0 && (fds[0].revents & POLLIN))
        {
         
            read(to_driver, buffer, sizeof(buffer));
            
            if (strncmp(buffer, "TASK", 4) == 0)
            {
                int task_time;
                sscanf(buffer, "TASK %d", &task_time);
                
                if (time_remaining > 0)
                {
                   
                    char busy_response[50];
                    sprintf(busy_response, "Busy %d", time_remaining);
                    write(from_driver, busy_response, strlen(busy_response) + 1);
                }
                else
                {
                    time_remaining = task_time;
                    write(from_driver, "OK", 3);
                    printf("Водитель %d: начал задачу на %d секунд\n", getpid(), task_time);
                }
            }
            else if (strcmp(buffer, "STATUS") == 0)
            {
                if (time_remaining > 0)
                {
                    char status[50];
                    sprintf(status, "Busy %d", time_remaining);
                    write(from_driver, status, strlen(status) + 1);
                }
                else
                {
                    write(from_driver, "Available", 10);
                }
            }
        }
        else if (ret == 0)
        {
         
            if (time_remaining > 0)
            {
                time_remaining--;
                if (time_remaining == 0)
                {
                    printf("Водитель %d: задача завершена\n", getpid());
                }
            }
        }
    }
}

void get_status(int pid)
{
    int found = 0;
    for (int i = 0; i < NUM_DRIVERS; i++)
    {
        if (drivers_list[i].pid == pid && !drivers_list[i].available)
        {
            found = 1;
            write(drivers_list[i].to_driver, "STATUS", 7);
            char response[50];
            read(drivers_list[i].from_driver, response, sizeof(response));
            if (strncmp(response, "Busy", 4) == 0)
            {
                int time_remaining;
                sscanf(response, "Busy %d", &time_remaining);
                printf("Водитель %d: Занят на время %d\n", pid, time_remaining);
            }
            else
            {
                printf("Водитель %d: Свободен\n", pid);
            }
            break;
        }
    }
    if (!found)
    {
        printf("Водитель с PID %d не найден\n", pid);
    }
}

void get_drivers()
{
    int count = 0;
    for (int i = 0; i < NUM_DRIVERS; i++)
    {
        if (drivers_list[i].available == false)
        {
            count++;
            write(drivers_list[i].to_driver, "STATUS", 7);
            char response[50];
            read(drivers_list[i].from_driver, response, sizeof(response));
            if (strncmp(response, "Busy", 4) == 0)
            {
                int time_remaining;
                sscanf(response, "Busy %d", &time_remaining);
                printf("Водитель %d: Занят на время %d\n", drivers_list[i].pid, time_remaining);
            }
            else
            {
                printf("Водитель %d: Свободен\n", drivers_list[i].pid);
            }
        }
    }
    if (count == 0)
    {
        printf("Нет активных водителей\n");
    }
}

void cleanup()
{
    if (drivers_list != NULL)
    {
        for (int i = 0; i < NUM_DRIVERS; i++)
        {
            if (!drivers_list[i].available)
            {
                close(drivers_list[i].to_driver);
                close(drivers_list[i].from_driver);
                kill(drivers_list[i].pid, SIGTERM);
                pthread_mutex_destroy(&drivers_list[i].mutex);
            }
        }
        free(drivers_list);
        drivers_list = NULL;
    }
}