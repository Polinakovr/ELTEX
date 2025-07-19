#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define MAX_SHOP 5
#define MAX_BUYER 3
FILE *file;
int shop[MAX_SHOP];
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t shop_mutex[MAX_SHOP];
bool flagsloader = true;
void *load(void *)
{
    while (flagsloader == true)
    {
        for (int i = 0; i < MAX_SHOP; i++)
        {
            if (pthread_mutex_trylock(&shop_mutex[i]) == 0)
            {
                shop[i] = shop[i] + 5000;
                fprintf(file,"Погрузчик положил в магазин %d 5000\n", i);
                pthread_mutex_unlock(&shop_mutex[i]);
            }

            sleep(1);
            if (flagsloader == false)
            {
                pthread_exit(NULL);
            }
        }
    }
}
void *routine(void *n)
{
    int *array = (int *)n;
    int *need = &array[0];
    int index = array[1];
    while ((*need) != 0)
    {
        for (int i = 0; i < MAX_SHOP; i++)
        {
            if (pthread_mutex_trylock(&shop_mutex[i]) == 0)
            {
                if (shop[i] == 0)
                {
                    pthread_mutex_unlock(&shop_mutex[i]);
                    continue;
                }
                fprintf(file,"Покупатель %d: Зашел в магазин %d, там было %d товаров\n", index, i, shop[i]);
                if (shop[i] <= (*need))
                {
                    *need = (*need) - shop[i];
                    fprintf(file,"Покупатель %d: Взял все %d товаров. Моя потребность теперь %d\n", index, shop[i], *need);
                    shop[i] = 0;
                }
                else
                {
                    fprintf(file,"Покупатель %d: Взял %d товаров. Моя потребность теперь 0\n", index, *need);
                    shop[i] = shop[i] - (*need);

                    (*need) = 0;
                }
                pthread_mutex_unlock(&shop_mutex[i]);
                fprintf(file,"Покупатель %d: Засыпаю\n", index);
                sleep(2);
                if ((*need) == 0)
                {
                    fprintf(file,"Покупатель %d: Потребность удовлетворена, завершаю работу\n", index);
                    pthread_exit(NULL);
                }
            }
        }
    }
}
int main()
{
    srand(time(NULL));
    pthread_t thread[MAX_BUYER];
    pthread_t loader;
    int need_arr[MAX_BUYER][2];
    if ((file = fopen("task.txt", "w")) ==NULL)
    {
        perror("Ошибка открытия файла");
        exit(1);
    }
    for (int i = 0; i < MAX_SHOP; i++)
    {
        shop[i] = rand() % 10001;
        fprintf(file,"Магазин %d: %d товаров\n", i, shop[i]);
    }
    for (int i = 0; i < MAX_SHOP; i++)
    {
        pthread_mutex_init(&shop_mutex[i], NULL);
    }
    pthread_create(&loader, NULL, load, NULL);
    for (int i = 0; i < MAX_BUYER; i++)
    {
        int need = rand() % 100001;
        need_arr[i][0] = need;
        need_arr[i][1] = i;
        fprintf(file,"Создан покупатель %d с потребностью %d\n", need_arr[i][1], need_arr[i][0]);
        pthread_create(&thread[i], NULL, routine, (void *)&need_arr[i]);
    }

    for (int i = 0; i < MAX_BUYER; i++)
    {
        pthread_join(thread[i], NULL);
    }
    flagsloader = false;
    pthread_join(loader, NULL);
    for (int i = 0; i < MAX_SHOP; i++)
    {
        pthread_mutex_destroy(&shop_mutex[i]);
    }
    fprintf(file,"Все покупатели удовлетворены, работа завершена\n");
      fclose(file);
   
    return 0;
}