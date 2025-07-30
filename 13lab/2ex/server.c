#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define MAX_MESSAGE 50
#define USER_NAME 20
#define MAX_CLIENT 10

struct users {
    char name[USER_NAME];
    char text[MAX_MESSAGE];
};

struct users *list = NULL;
unsigned int user_count = 0;

void *text_handler(void *arg) {
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10, 
        .mq_msgsize = sizeof(struct users),
        .mq_curmsgs = 0
    };

    mqd_t official = *(mqd_t *)arg;
    mqd_t message = mq_open("/message", O_CREAT | O_RDONLY, 0666, &attr);
    if (message == (mqd_t)-1) {
        perror("Error creating message queue");
        exit(EXIT_FAILURE);
    }

    struct users users_now;
    while (1) {
        if (mq_receive(message, (char *)&users_now, sizeof(struct users), NULL) == -1) {
            perror("Error receiving message");
            mq_close(message);
            mq_close(official);
            exit(EXIT_FAILURE);
        }

        if (mq_send(official, (char *)&users_now, sizeof(struct users), 5) == -1) {
            perror("Error sending message to duty");
            mq_close(message);
            mq_close(official);
            exit(EXIT_FAILURE);
        }
    }

    mq_close(message);
    return NULL;
}

void *name_handler(void *arg) {
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = sizeof(struct users),
        .mq_curmsgs = 0
    };

    mqd_t official = *(mqd_t *)arg;
    mqd_t queue_name = mq_open("/name", O_CREAT | O_RDONLY, 0666, &attr);
    if (queue_name == (mqd_t)-1) {
        perror("Error creating name queue");
        exit(EXIT_FAILURE);
    }

    struct users user_buffer;
    while (1) {
        if (mq_receive(queue_name, (char *)&user_buffer, sizeof(struct users), NULL) == -1) {
            perror("Error receiving name message");
            mq_close(queue_name);
            mq_close(official);
            exit(EXIT_FAILURE);
        }
        

        if (user_count >= MAX_CLIENT) {
            fprintf(stderr, "Too many clients\n");
            continue;
        }

        struct users *temp = realloc(list, (user_count + 1) * sizeof(struct users));
        if (temp == NULL) {
            perror("Error reallocating memory");
            mq_close(queue_name);
            mq_close(official);
            exit(EXIT_FAILURE);
        }
        list = temp;

        strncpy(list[user_count].name, user_buffer.name, USER_NAME);
        list[user_count].name[USER_NAME - 1] = '\0';
        memset(list[user_count].text, 0, MAX_MESSAGE);

        if (mq_send(official, (char *)&list[user_count], sizeof(struct users), 6) == -1) {
            perror("Error sending user info");
            mq_close(queue_name);
            mq_close(official);
            exit(EXIT_FAILURE);
        }

        user_count++;
    }

    mq_close(queue_name);
    return NULL;
}

int main() {
    mq_unlink("/duty");
    mq_unlink("/name");
    mq_unlink("/message");

    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = sizeof(struct users),
        .mq_curmsgs = 0
    };

    pthread_t text_thread, name_thread;
    mqd_t duty = mq_open("/duty", O_CREAT | O_WRONLY, 0666, &attr);
    if (duty == (mqd_t)-1) {
        fprintf(stderr, "Error creating duty queue: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if (pthread_create(&text_thread, NULL, text_handler, &duty) != 0) {
        perror("Error creating text thread");
        mq_close(duty);
        mq_unlink("/duty");
        return EXIT_FAILURE;
    }

    if (pthread_create(&name_thread, NULL, name_handler, &duty) != 0) {
        perror("Error creating name thread");
        pthread_cancel(text_thread);
        mq_close(duty);
        mq_unlink("/duty");
        return EXIT_FAILURE;
    }

    pthread_join(text_thread, NULL);
    pthread_join(name_thread, NULL);

    free(list);
    mq_close(duty);
    mq_unlink("/duty");
    mq_unlink("/name");
    mq_unlink("/message");
    return 0;
}