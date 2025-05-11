#include <stdio.h>
#include <string.h>
struct abonent
{
    char name[10];
    char second_name[10];
    char tel[10];
};
void add_abonent(int *i, struct abonent *directory)
{
    if (*i > 100)
    {
        printf("Массив переполнен, запись не может произойти");
    }
    else
    {
        printf("\n Имя \n ");
        scanf("%s", directory[*i].name);
        printf("\n Фамилия\n ");
        scanf("%s", directory[*i].second_name);
        printf("\n Телефон \n");
        scanf("%s", directory[*i].tel);
        (*i)++;
    }
}
void delete_abonent(struct abonent *directory){
    char dir_name[10];
    char dir_secondname[10];
    printf("Введите данные Имя и Фамилию, чтобы удалить абонента\n ");
    printf("Имя\n ");
    scanf("%s", dir_name);
    printf("Фамилия\n ");
    scanf("%s", dir_secondname);
    for (int j = 0; j <= 100; j++)
    {
        if (strcmp(directory[j].name, dir_name) == 0)
            if (strcmp(directory[j].second_name, dir_secondname) == 0)
            {
                strcpy(directory[j].name, "0");
                strcpy(directory[j].second_name, "0");
                strcpy(directory[j].tel, "0");
            }
    }
}
void look_abonent(struct abonent *directory){
    char dir1_name[10];
    printf("Введите  Имя , чтобы найти абонента \n");
    printf("Имя \n");
    scanf("%s", dir1_name);
    for (int j = 0; j <= 100; j++)
    {
        if (strcmp(directory[j].name, dir1_name) == 0)
        {
            printf("Имя: %s ", directory[j].name);
            printf("Фамилия: %s ", directory[j].second_name);
            printf("Телефон: %s ", directory[j].tel);
            printf("\n");
        }
    }
}
void print(struct abonent *directory,int *i){
    for (int j = 0; j <= (*i) - 1; j++)
    {

        printf("Имя: %s ", directory[j].name);
        printf("Фамилия: %s ", directory[j].second_name);
        printf("Телефон: %s ", directory[j].tel);
        printf("\n");
    }
}
int main()
{

    int x = 1;
    int i = 0;
    struct abonent directory[100] = {{0}};
    while (x != 5 && x >= 1 && x <= 4)
    {
        printf(" 1) Добавить абонента \n 2) Удалить абонента \n 3) Поиск абонентов по имени \n 4) Вывод всех записей \n 5) Выход");
        x = 0;
        scanf("\n %d", &x);

        switch (x)
        {
        case 1:

            add_abonent(&i, directory);
            break;
        case 2:
            delete_abonent(directory);
            break;
        case 3:
          look_abonent(directory);
            break;
        case 4:
          print(directory,&i);
            break;
        default:
            break;
        }
    }
    return 0;
}