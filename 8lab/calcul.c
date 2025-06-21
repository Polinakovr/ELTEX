#include <stdlib.h>
#include "add.h"
#include "div.h"
#include "sub.h"
#include "mul.h"
#include "stdio.h"
int main()
{
    int x,a,b;
    while (1)
    {
        printf("1) Сложение \n \
2) Вычитание \n \
3) Умножение \n \
4) Деление \n \
5) Выход");
        scanf("%d", &x);
        if (x==5){
            return 0;
        }
        scanf("%d %d", &a, &b);
        switch (x)
        {
        case 1:
            add(&a,&b);
            break;
        case 2:
             sub(&a,&b);
            break;
        case 3:
             mul(&a,&b);
            break;
        case 4:
            divide(&a,&b);
            break;
        default:
            break;
        }
    }
}