#include "div.h"
#include "stdio.h"
#include <stdlib.h>
void divide(int *a,int *b){
    if((*b)==0){
       printf("Ошибка деления ");
       exit (1);
    }
    printf(" \n Результат: %.2f \n ", (float)(*a)/(float)(*b));
}