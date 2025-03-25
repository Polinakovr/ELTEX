#include <stdlib.h>
#include <stdio.h>
void triangle(int p)
{
    int arr3[p][p];
    for (int i = 0; i < p; i++)
    {
        for (int j = p - 1; j >= 0; j--)
        {
            if (i == j || i > j)
            {
                arr3[i][j] = 1;
            }
            else
            {
                arr3[i][j] = 0;
            }
            printf("%d", arr3[i][j]);
        }
        printf("\n");
    }
}
void snake(int n)
{
    int arr3[n][n];
    int count = 0;
    int i = 0;
    int j = 0;
    int left = 0;
    int right = n - 1;
    int row = 0;
    int b = n - 1;
    while (row <= b && left <= right)
    {
        for (j = left; j <= right; j++)
        {
            arr3[row][j] = count++;
        }
        row++;
        i = row;

        while (i <= b)
        {
            arr3[i][right] = count++;
            i++;
        }
        right--;
        if (row <= b)
        {
            for (j = right; j >= left; j--)
            {
                arr3[b][j] = count++;
            }
            b--;
            i = b;
        }
        if (left <= right)
        {
            while (i >= row)
            {
                arr3[i][left] = count++;
                i--;
            }
            left++;
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", arr3[i][j]);
        }
        printf("\n");
    }
}

void reverse(int arr[], int n)
{
    int arr1[n];
    for (int i = n - 1, j = 0; i >= 0; i--, j++)
    {
        arr1[j] = arr[i];
    }
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr1[i]);
    }
}
void sq_matrix(int n)
{
    int arr[n][n];
    int c = 1;
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            arr[i][j] = c++;
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
}
int main()
{
    int n, k, p;
    printf("1) Введите размер матрицы: ");
    scanf("%d", &n);
    sq_matrix(n);
    printf("Введите размер матрицы: ");
    scanf("%d", &n);
    printf("\nВведите значение элементов в матрице размером %d \n", n);

    int arr[n];
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &k);
        arr[i] = k;
    }
    reverse(arr, n);
    printf("Введите размер треугольной матрицы, этот размер будет использоваться и для змейки: ");
    scanf("%d", &p);
    triangle(p);
    snake(p);
}