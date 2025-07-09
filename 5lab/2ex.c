#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int IsPassOk(void);
int main(void)
{
    int PwStatus;
    puts("Enter password:");
    PwStatus = IsPassOk();
    if (PwStatus == 0)
    {
        printf("Bad password!\n");
        exit(1);
    }
    else
    {
        printf("Access granted!\n");
        // Строка для которой нужно выяснить адрес
        /* адрес строчки 0x00000000004011d8.
        "A"*20 + b"\xd8\x11\x40\x00\x00\x00\x00\x00
       заполняем Pass(12 байт) и перезаписваем rbp(8 байт), + адрес возврата
       ./c < payload.txt
*/
    }
    return 0;
}
int IsPassOk(void)
{
    char Pass[12];
    gets(Pass);
    return 0 == strcmp(Pass, "test");
}