#include <stdio.h>
int binary_changebyte(int x, int b)
{
    x = x & ~(0xFF >> 16);
    x = x | (b << 16);
    printf("%d", x);
    printf("\n");
    return x;
}
int count(int c)
{
    int counter = 0;
    for (int i = c; i >= 0; i--)
    {

        int binary = (c >> i) & 1;
        if (binary == 1)
        {
            counter += 1;
        }
    }
    printf("%d", counter);
    printf("\n");
    return counter;
}
void binaryminus(int y)
{
    y = -y;
    for (int i = y; i >= 0; i--)
    {
        int bit = ~(y >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}
void binary(int x)
{
    for (int i = x; i >= 0; i--)
    {
        int binary = (x >> i) & 1;
        printf("%d", binary);
    }
    printf("\n");
}
int main()
{
    printf("1) Введите целое положительное число:");
    int x, y, c, b;
    scanf("%d", &x);
    binary(x);
    printf("\n 2) Введите целое отрицательное число:");
    scanf("%d", &y);
    binaryminus(y);
    printf("\n 3) Введите целое положительное число для поиска единиц : ");
    scanf("%d", &c);
    count(c);
    printf("\n 4) Введите число для замены 3 байта числа из 1) : ");
    scanf("%d", &b);
    binary_changebyte(x, b);
}