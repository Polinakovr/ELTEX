#ifndef DRIVER_H
#define DRIVER_H

#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_DRIVERS 10

void init();
pid_t create_driver();
void send_task(int pid, int seconds);
void get_status(int pid);
void get_drivers();
void task(int to_driver, int from_driver);
void cleanup();
void* timer_thread(void* arg);

#endif