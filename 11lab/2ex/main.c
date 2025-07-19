#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define N 5
int a;
pthread_mutex_t m1=PTHREAD_MUTEX_INITIALIZER;
void *printindex(void *i)
{
    int *args = (int *)i;
    
    for (int j = 0; j < 180000; j++)
    {  pthread_mutex_lock(&m1);
        a++;
         pthread_mutex_unlock(&m1);
    }
    return NULL;
}
int main()
{
    pthread_t thread[N];
    int v[N];
    for (int i = 0; i < N; i++)
    {
        v[i] = i;
        pthread_create(&thread[i], NULL, printindex, (void *)&v[i]);
    }
    for (int s = 0; s < N; s++)
    {
        pthread_join(thread[s], (void **)&s);
    }
    printf("%d",a);
}