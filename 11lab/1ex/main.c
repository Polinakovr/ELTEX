#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define N 5
void *printindex(void *i)
{
    int *args = (int *)i;
    printf("%d",*args);
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
}