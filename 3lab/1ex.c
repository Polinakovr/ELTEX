#include <stdio.h>
int main()
{
    int c, k;
    printf("Введите положительное число");
    scanf("%d", &c);
    printf("3 байт");
    scanf("%d", &k);
    char *ptr = (char *)&c;
    ptr = ptr + 2;
    *ptr = k;
    printf("%d", c);
    return 0;
}