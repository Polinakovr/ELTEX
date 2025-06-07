#include "library.h"
void add_abonent(size_t *i, Node **head, Node **tail)
{
    if (*i > 100)
    {
        printf("Массив переполнен, запись не может произойти");
    }
    Node *ptr = malloc(sizeof(Node));
    printf("\n Имя\n ");
    scanf("%s", ptr->value.name);
    printf("\n Фамилия\n ");
    scanf("%s", ptr->value.second_name);
    printf("\n Телефон \n");
    scanf("%s", ptr->value.tel);
    if ((*head) == NULL)
    {
        *head = ptr;
        *tail = ptr;
        (*head)->prev = NULL;
        (*head)->next = NULL;
        (*i)++;
    }
    else
    {
        ptr->prev = (*tail);
        ptr->next = NULL;

        (*tail)->next = ptr;
        (*tail) = ptr;
        (*i)++;
    }
}

void delete_abonent(size_t *i, Node **head, Node **tail)
{
    char dir_name[10];
    char dir_secondname[10];
    printf("Введите данные имя и фамилию, чтобы удалить абонента\n ");
    printf("Имя\n ");
    scanf("%s", dir_name);
    printf("Фамилия\n ");
    scanf("%s", dir_secondname);
    Node *current = (*head);
    while (current != NULL)
    {
        Node *t = current->next;

        if (strcmp(current->value.name, dir_name) == 0)

            if (strcmp(current->value.second_name, dir_secondname) == 0)

            {
                if (current->prev != NULL)
                {
                    current->prev->next = current->next;
                }
                else
                {
                    *head = current->next;
                }
                if (current->next != NULL)
                {
                    current->next->prev = current->prev;
                }
                else
                {
                    *tail = current->prev;
                }

                free(current);
            }
        current = t;
    }
    (*i)--;
}
void look_abonent(Node **head)
{
    char dir1_name[10];
    printf("Введите  Имя , чтобы найти абонента \n");
    printf("Имя \n");
    scanf("%s", dir1_name);
    Node *current = (*head);
    while (current != NULL)
    {
        if (strcmp(current->value.name, dir1_name) == 0)
        {
            printf("Имя: %s ", current->value.name);
            printf("Фамилия: %s ", current->value.second_name);
            printf("Телефон: %s ", current->value.tel);
            printf("\n");
        }
        current = current->next;
    }
}
void print(Node **head)
{

    Node *current = (*head);
    while (current != NULL)
    {
        printf("Имя: %s ", current->value.name);
        printf("Фамилия: %s ", current->value.second_name);
        printf("Телефон: %s ", current->value.tel);
        printf("\n");
        current = current->next;
    }
}
int main()
{

    int x = 1;
    size_t i = 0;
    Node *head = NULL;
    Node *tail = NULL;
    while (x != 5 && x >= 1 && x <= 4)
    {
        printf(" 1) Добавить абонента \n 2) Удалить абонента \n 3) Поиск абонентов по имени \n 4) Вывод всех записей \n 5) Выход");
        x = 0;
        scanf("\n %d", &x);
        switch (x)
        {
        case 1:

            add_abonent(&i, &head, &tail);
            break;
        case 2:
            delete_abonent(&i, &head, &tail);
            break;
        case 3:
            look_abonent(&head);
            break;
        case 4:
            print(&head);
            break;
        default:
            break;
        }
    }
    Node *current = head;
    while (current != NULL)
    {
        Node *temp = current->next;
        free(current);
        current = temp;
    }
    return 0;
}