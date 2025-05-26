#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Equation method or matrix multiplication?\n");
    int choice1;
    scanf("%d", &choice1);

    if (choice1 == 0)
    {
        printf("wait for the next version of the program.\n");
    }
    else if (choice1 == 1)
    {
        printf("Automatically fill the matrix with random numbers? yes == 1.\n");
        int n;
        scanf("%d", &n);

        if (n == 0)
        {
            int n, i, j, k;
            printf("Enter the size of the matrix (n for n x n): ");
            scanf("%d", &n);

            // Dynamic allocation
            int **a = (int **)malloc(n * sizeof(int *));
            int **b = (int **)malloc(n * sizeof(int *));
            int **result = (int **)malloc(n * sizeof(int *));
            for (i = 0; i < n; i++)
            {
                a[i] = (int *)malloc(n * sizeof(int));
                b[i] = (int *)malloc(n * sizeof(int));
                result[i] = (int *)calloc(n, sizeof(int));
            }

            printf("Enter elements of first %dx%d matrix:\n", n, n);
            for (i = 0; i < n; i++)
            {
                for (j = 0; j < n; j++)
                {
                    printf("a[%d][%d]: ", i, j);
                    scanf("%d", &a[i][j]);
                }
            }

            printf("Enter elements of second %dx%d matrix:\n", n, n);
            for (i = 0; i < n; i++)
            {
                for (j = 0; j < n; j++)
                {
                    printf("b[%d][%d]: ", i, j);
                    scanf("%d", &b[i][j]);
                }
            }

            // Matrix multiplication
            for (i = 0; i < n; i++)
            {
                for (j = 0; j < n; j++)
                {
                    for (k = 0; k < n; k++)
                    {
                        result[i][j] += a[i][k] * b[k][j];
                    }
                }
            }

            printf("Resultant matrix:\n");
            for (i = 0; i < n; i++)
            {
                for (j = 0; j < n; j++)
                {
                    printf("%d ", result[i][j]);
                }
                printf("\n");
            }

            // Free memory
            for (i = 0; i < n; i++)
            {
                free(a[i]);
                free(b[i]);
                free(result[i]);
            }
            free(a);
            free(b);
            free(result);
        }
        else if (n == 1)
        {
            int n, i, j, k;
            printf("Enter the size of the matrix (n for n x n): ");
            scanf("%d", &n);

            // Dynamic allocation
            int **a = (int **)malloc(n * sizeof(int *));
            int **b = (int **)malloc(n * sizeof(int *));
            int **result = (int **)malloc(n * sizeof(int *));
            for (i = 0; i < n; i++)
            {
                a[i] = (int *)malloc(n * sizeof(int));
                b[i] = (int *)malloc(n * sizeof(int));
                result[i] = (int *)calloc(n, sizeof(int));
            }

            // Fill matrices with random numbers
            for (i = 0; i < n; i++)
            {
                for (j = 0; j < n; j++)
                {
                    a[i][j] = rand() % 10; // Random numbers between 0 and 9
                    b[i][j] = rand() % 10;
                }
            }

            // Matrix multiplication
            for (i = 0; i < n; i++)
            {
                for (j = 0; j < n; j++)
                {
                    for (k = 0; k < n; k++)
                    {
                        result[i][j] += a[i][k] * b[k][j];
                    }
                }
            }

            printf("Resultant matrix:\n");
            for (i = 0; i < n; i++)
            {
                for (j = 0; j < n; j++)
                {
                    printf("%d ", result[i][j]);
                }
                printf("\n");
            }

            // Free memory
            for (i = 0; i < n; i++)
            {
                free(a[i]);
                free(b[i]);
                free(result[i]);
            }
            free(a);
            free(b);
            free(result);
        }
        else
        {
            printf("Invalid choice. Please enter 0 or 1.\n");
        }
    }
    else
    {
        printf("Invalid choice. Please enter 0 or 1.\n");
        return 1;
    }

    return 0;
}