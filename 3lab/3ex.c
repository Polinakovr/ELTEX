#include <stdio.h>
int main()
{
    int arr[10];
    int count = 1;
    int *ptr = arr;
    for (int i = 0; i < 10; i++)
    {
        arr[i] = count++;
        printf("%d", *ptr);
        ptr++;
    }
}