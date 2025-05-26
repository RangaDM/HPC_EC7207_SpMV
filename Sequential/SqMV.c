#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Select method:\n");
    printf("1 : Solving a system of equations.\n");
    printf("2 : Matrix multiplication.\n");
    int choice1;
    scanf("%d", &choice1);

    if (choice1 == 1)
    {
        printf("You chose: System of equations.\n");
        printf("Enter the number of equations (variables): ");
        int n;
        scanf("%d", &n);

        int **coeff = (int **)malloc(n * sizeof(int *));
        for (int i = 0; i < n; i++)
        {
            coeff[i] = (int *)malloc(n * sizeof(int));
        }
        int *constants = (int *)malloc(n * sizeof(int));

        printf("Equation format : aX + bY + cZ = d\n");
        printf("Enter the coefficients and the constant term for each equation.\n");
        printf("For each equation, enter %d coefficients followed by the constant term:\n", n);
        for (int i = 0; i < n; i++)
        {
            printf("Equation %d:\n", i + 1);
            for (int j = 0; j < n; j++)
            {
                printf("  Coefficient of variable %d: ", j + 1);
                scanf("%d", &coeff[i][j]);
            }
            printf("  Constant term: ");
            scanf("%d", &constants[i]);
        }

        printf("\nCoefficient matrix:\n");
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                printf("%d ", coeff[i][j]);
            }
            printf("\n");
        }

        printf("Constants vector:\n");
        for (int i = 0; i < n; i++)
        {
            printf("%d\n", constants[i]);
        }

        {
            printf("\nSolving system using LU factorization...\n");

            // Convert the coefficient matrix and constants vector to double arrays
            double **A = malloc(n * sizeof(double *));
            for (int i = 0; i < n; i++)
            {
                A[i] = malloc(n * sizeof(double));
                for (int j = 0; j < n; j++)
                {
                    A[i][j] = coeff[i][j];
                }
            }
            double *b = malloc(n * sizeof(double));
            for (int i = 0; i < n; i++)
            {
                b[i] = constants[i];
            }

            // Allocate memory for L and U matrices
            double **L = malloc(n * sizeof(double *));
            double **U = malloc(n * sizeof(double *));
            for (int i = 0; i < n; i++)
            {
                L[i] = calloc(n, sizeof(double));
                U[i] = calloc(n, sizeof(double));
            }

            // Perform LU Decomposition without pivoting
            for (int i = 0; i < n; i++)
            {
                // Compute U's row
                for (int j = i; j < n; j++)
                {
                    double sum = 0;
                    for (int k = 0; k < i; k++)
                    {
                        sum += L[i][k] * U[k][j];
                    }
                    U[i][j] = A[i][j] - sum;
                }

                // Set the diagonal of L to 1
                L[i][i] = 1.0;

                // Compute L's column
                for (int j = i + 1; j < n; j++)
                {
                    double sum = 0;
                    for (int k = 0; k < i; k++)
                    {
                        sum += L[j][k] * U[k][i];
                    }
                    if (U[i][i] == 0)
                    {
                        printf("Error: Division by zero in LU factorization.\n");
                        exit(1);
                    }
                    L[j][i] = (A[j][i] - sum) / U[i][i];
                }
            }

            // Print L and U matrices

            // printf("\nL matrix:\n");
            // for (int i = 0; i < n; i++)
            // {
            //     for (int j = 0; j < n; j++)
            //     {
            //         printf("%lf ", L[i][j]);
            //     }
            //     printf("\n");
            // }

            // printf("\nU matrix:\n");
            // for (int i = 0; i < n; i++)
            // {
            //     for (int j = 0; j < n; j++)
            //     {
            //         printf("%lf ", U[i][j]);
            //     }
            //     printf("\n");
            // }

            // Forward substitution: solve Ly = b
            double *y = malloc(n * sizeof(double));
            for (int i = 0; i < n; i++)
            {
                double sum = 0;
                for (int j = 0; j < i; j++)
                {
                    sum += L[i][j] * y[j];
                }
                y[i] = b[i] - sum;
            }

            // Backward substitution: solve Ux = y
            double *x = malloc(n * sizeof(double));
            for (int i = n - 1; i >= 0; i--)
            {
                double sum = 0;
                for (int j = i + 1; j < n; j++)
                {
                    sum += U[i][j] * x[j];
                }
                if (U[i][i] == 0)
                {
                    printf("Error: Division by zero in backward substitution.\n");
                    exit(1);
                }
                x[i] = (y[i] - sum) / U[i][i];
            }

            // Print the solution
            printf("\nSolution:\n");
            for (int i = 0; i < n; i++)
            {
                printf("Variable %d: %lf\n", i + 1, x[i]);
            }

            // Free allocated memory
            for (int i = 0; i < n; i++)
            {
                free(A[i]);
                free(L[i]);
                free(U[i]);
            }
            free(A);
            free(L);
            free(U);
            free(b);
            free(y);
            free(x);
        }

        for (int i = 0; i < n; i++)
        {
            free(coeff[i]);
        }
        free(coeff);
        free(constants);
    }
    else if (choice1 == 2)
    {
        printf("You chose: Matrix multiplication.\n\n");
        printf("Select method:\n");
        printf("1 : Manually fill.\n");
        printf("2 : Automatically fill.\n");
        int n;
        scanf("%d", &n);

        if (n == 1)
        {
            int size, i, j;
            printf("Enter the size of the vector and matrix (n for n x n): ");
            scanf("%d", &size);

            // Allocate vector a and matrix b
            int *a = (int *)malloc(size * sizeof(int));
            int **b = (int **)malloc(size * sizeof(int *));
            int *result = (int *)calloc(size, sizeof(int));
            for (i = 0; i < size; i++)
            {
                b[i] = (int *)malloc(size * sizeof(int));
            }

            printf("Enter elements of the vector (size %d):\n", size);
            for (i = 0; i < size; i++)
            {
                printf("a[%d]: ", i);
                scanf("%d", &a[i]);
            }
            printf("Vector a:\n");
            for (i = 0; i < size; i++)
            {
                printf("%d ", a[i]);
            }
            printf("\n\n");

            printf("Enter elements of the %dx%d matrix:\n", size, size);
            for (i = 0; i < size; i++)
            {
                for (j = 0; j < size; j++)
                {
                    printf("b[%d][%d]: ", i, j);
                    scanf("%d", &b[i][j]);
                }
            }

            printf("Matrix b:\n");
            for (i = 0; i < size; i++)
            {
                for (j = 0; j < size; j++)
                {
                    printf("%d ", b[i][j]);
                }
                printf("\n");
            }
            printf("\n");

            // Vector-matrix multiplication: result = b * a
            for (i = 0; i < size; i++)
            {
                for (j = 0; j < size; j++)
                {
                    result[i] += a[j] * b[j][i];
                }
            }

            printf("Resultant vector:\n");
            for (i = 0; i < size; i++)
            {
                printf("%d ", result[i]);
            }
            printf("\n");

            // Free memory
            free(a);
            for (i = 0; i < size; i++)
            {
                free(b[i]);
            }
            free(b);
            free(result);
        }
        else if (n == 2)
        {
            int size, i, j;
            printf("Enter the size of the vector and matrix (n for n x n): ");
            scanf("%d", &size);

            // Allocate vector a and matrix b
            int *a = (int *)malloc(size * sizeof(int));
            int **b = (int **)malloc(size * sizeof(int *));
            int *result = (int *)calloc(size, sizeof(int));
            for (i = 0; i < size; i++)
            {
                b[i] = (int *)malloc(size * sizeof(int));
            }

            // Fill vector and matrix with random numbers
            printf("Vector a (randomly filled):\n");
            for (i = 0; i < size; i++)
            {
                a[i] = rand() % 10;
                printf("%d ", a[i]);
            }
            printf("\n");

            printf("Matrix b (randomly filled):\n");
            for (i = 0; i < size; i++)
            {
                for (j = 0; j < size; j++)
                {
                    b[i][j] = rand() % 10;
                    printf("%d ", b[i][j]);
                }
                printf("\n");
            }

            // Vector-matrix multiplication: result = a * b
            for (i = 0; i < size; i++)
            {
                for (j = 0; j < size; j++)
                {
                    result[i] += a[j] * b[j][i];
                }
            }

            printf("Resultant vector (a * b) :\n");
            for (i = 0; i < size; i++)
            {
                printf("%d  ", result[i]);
            }
            printf("\n");

            // Free memory
            free(a);
            for (i = 0; i < size; i++)
            {
                free(b[i]);
            }
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
    // Free memory for the coefficient matrix and constants vector

    return 0;
}