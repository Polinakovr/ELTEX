#ifndef __LIBRARY__
#define __LIBRARY__
#include <stdio.h>
#include <string.h>
#include <malloc.h>
typedef struct
{
    char name[10];
    char second_name[10];
    char tel[10];
} abonent;
typedef struct Node
{
    abonent value;
    struct Node *prev;
    struct Node *next;
} Node;
void add_abonent(size_t *i, Node **head, Node **tail);
void delete_abonent(size_t *i, Node **head, Node **tail);
void look_abonent(Node **head);
void print(Node **head);

#endif