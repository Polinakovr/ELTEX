#include "driver.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PID_NUM_SIZE 10

int main()
{
    init();

    while (1)
    {
        int x;
        char input[PID_NUM_SIZE];

        printf("\nВыберите одну из команд:\n");
        printf("1) Создать водителя\n");
        printf("2) Отправить задачу\n");
        printf("3) Получить статус определенного водителя\n");
        printf("4) Получить статус всех водителей\n");
        printf("5) Выход\n");
        printf("Введите номер команды: ");

        if (scanf("%d", &x) != 1)
        {
            printf("Ошибка ввода\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        while (getchar() != '\n')
            ;

        switch (x)
        {
            case 1:
            {
                pid_t pid = create_driver();
                if (pid != -1)
                {
                    printf("Создан водитель с PID: %d\n", pid);
                }
                else
                {
                    printf("Не удалось создать водителя\n");
                }
            }
            break;

            case 2:
            {
                char pid_str[PID_NUM_SIZE];
                int second;

                printf("Введите PID водителя: ");
                if (fgets(pid_str, PID_NUM_SIZE, stdin) == NULL)
                {
                    printf("Ошибка чтения PID: %s\n", strerror(errno));
                    break;
                }
                pid_str[strcspn(pid_str, "\n")] = '\0';

                printf("Введите время в секундах: ");
                if (scanf("%d", &second) != 1)
                {
                    printf("Ошибка ввода времени\n");
                    while (getchar() != '\n')
                        ;
                    break;
                }
                while (getchar() != '\n')
                    ;

                if (second <= 0)
                {
                    printf("Время должно быть положительным числом\n");
                    break;
                }

                send_task(atoi(pid_str), second);
            }
            break;

            case 3:
            {
                char pid_str[PID_NUM_SIZE];

                printf("Введите PID водителя: ");
                if (fgets(pid_str, PID_NUM_SIZE, stdin) == NULL)
                {
                    printf("Ошибка чтения PID: %s\n", strerror(errno));
                    break;
                }
                pid_str[strcspn(pid_str, "\n")] = '\0';

                get_status(atoi(pid_str));
            }
            break;

            case 4: get_drivers(); break;

            case 5:
                printf("Выход из программы...\n");
                cleanup();
                exit(EXIT_SUCCESS);
                break;

            default: printf("Ошибка: ввели неправильное число (%d)\n", x); continue;
        }
    }

    return 0;
}