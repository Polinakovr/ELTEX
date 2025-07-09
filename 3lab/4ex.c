#include <stdio.h>
#include <stdlib.h>
int main()
{
    char arr[20];
    char arr1[10];
    int count = 0;
    int len = 0;
    int len1 = 0;
    int flag = 0;
    char *ptr = arr;
    printf("Введите строку");
    fgets(arr, sizeof(arr), stdin);
    printf("Введите подстроку");
    fgets(arr1, sizeof(arr1), stdin);
    while (arr[len] != '\0' && arr[len1] != '\n')
        len++;
    arr[len] = '\0';

    while (arr1[len1] != '\0' && arr1[len1] != '\n')
        len1++;
    arr1[len1] = '\0';
    for (int i = 0; i < len - len1; i++)
    {
        count = 0;
        for (int j = 0; j < len1; j++)
        {
            if (arr[i + j] == arr1[j])
            {
                count++;
            }
        }
        if (count == len1)
        {
            ptr = &arr[i];
            flag = 1;
            break;
        }
    }
    if (flag == 1)
    {
        printf("%p", (void *)ptr);
    }
    else
    {
        ptr = NULL;
    }
}