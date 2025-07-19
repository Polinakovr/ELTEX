#include "routines.h"
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
                fprintf(file, "Покупатель %d: Зашел в магазин %d, там было %d товаров\n", index, i, shop[i]);
                if (shop[i] <= (*need))
                {
                    *need = (*need) - shop[i];
                    fprintf(file, "Покупатель %d: Взял все %d товаров. Моя потребность теперь %d\n", index, shop[i],
                            *need);
                    shop[i] = 0;
                }
                else
                {
                    fprintf(file, "Покупатель %d: Взял %d товаров. Моя потребность теперь 0\n", index, *need);
                    shop[i] = shop[i] - (*need);

                    (*need) = 0;
                }
                pthread_mutex_unlock(&shop_mutex[i]);
                fprintf(file, "Покупатель %d: Засыпаю\n", index);
                sleep(2);
                if ((*need) == 0)
                {
                    fprintf(file, "Покупатель %d: Потребность удовлетворена, завершаю работу\n", index);
                    pthread_exit(NULL);
                }
            }
        }
    }
}