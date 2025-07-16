#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

double get_time()
{
#ifdef _WIN32
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / freq.QuadPart;
#else
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
#endif
}

int main()
{
    printf("Select method:\n1 : System of equations\n2 : Matrix multiplication\n");
    int choice;
    scanf("%d", &choice);

    if (choice == 1)
    {
        int n;
        printf("You chose: System of equations\n");
        printf("Enter number of variables: ");
        scanf("%d", &n);

        int **coeff = malloc(n * sizeof(int *));
        for (int i = 0; i < n; i++)
            coeff[i] = malloc(n * sizeof(int));
        int *constants = malloc(n * sizeof(int));

        printf("Enter coefficients and constants:\n");
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                printf("Coefficient of variable %d in equation %d: ", j + 1, i + 1);
                scanf("%d", &coeff[i][j]);
            }
            printf("Constant term for equation %d: ", i + 1);
            scanf("%d", &constants[i]);
        }

        double **A = malloc(n * sizeof(double *));
        double *b = malloc(n * sizeof(double));
        for (int i = 0; i < n; i++)
        {
            A[i] = malloc(n * sizeof(double));
            for (int j = 0; j < n; j++)
                A[i][j] = coeff[i][j];
            b[i] = constants[i];
        }

        double **L = malloc(n * sizeof(double *));
        double **U = malloc(n * sizeof(double *));
        for (int i = 0; i < n; i++)
        {
            L[i] = calloc(n, sizeof(double));
            U[i] = calloc(n, sizeof(double));
        }

        double start_time = get_time();

        for (int i = 0; i < n; i++)
        {
#pragma omp parallel for default(none) shared(U, L, A, i, n)
            for (int j = i; j < n; j++)
            {
                double sum = 0;
                for (int k = 0; k < i; k++)
                    sum += L[i][k] * U[k][j];
                U[i][j] = A[i][j] - sum;
            }

            L[i][i] = 1.0;

#pragma omp parallel for default(none) shared(L, U, A, i, n)
            for (int j = i + 1; j < n; j++)
            {
                double sum = 0;
                for (int k = 0; k < i; k++)
                    sum += L[j][k] * U[k][i];
                L[j][i] = (A[j][i] - sum) / U[i][i];
            }
        }

        double *y = malloc(n * sizeof(double));
        for (int i = 0; i < n; i++)
        {
            double sum = 0;
#pragma omp parallel for reduction(+ : sum) default(none) shared(L, y, i, b)
            for (int j = 0; j < i; j++)
                sum += L[i][j] * y[j];
            y[i] = b[i] - sum;
        }

        double *x = malloc(n * sizeof(double));
        for (int i = n - 1; i >= 0; i--)
        {
            double sum = 0;
#pragma omp parallel for reduction(+ : sum) default(none) shared(U, x, y, i, n)
            for (int j = i + 1; j < n; j++)
                sum += U[i][j] * x[j];
            x[i] = (y[i] - sum) / U[i][i];
        }

        double end_time = get_time();

        printf("\nSolution:\n");
        for (int i = 0; i < n; i++)
            printf("Variable %d = %.3lf\n", i + 1, x[i]);

        printf("Time taken for LU decomposition and solving: %.3f ms\n", (end_time - start_time) * 1000.0);

        for (int i = 0; i < n; i++)
        {
            free(A[i]);
            free(L[i]);
            free(U[i]);
            free(coeff[i]);
        }
        free(A);
        free(L);
        free(U);
        free(b);
        free(y);
        free(x);
        free(coeff);
        free(constants);
    }
    else if (choice == 2)
    {
        printf("You chose: Matrix multiplication\n");
        printf("1: Manual fill\n2: Auto random fill\n");
        int fill_choice;
        scanf("%d", &fill_choice);

        printf("Enter size of vector/matrix: ");
        int size;
        scanf("%d", &size);

        int *a = malloc(size * sizeof(int));
        int **b = malloc(size * sizeof(int *));
        for (int i = 0; i < size; i++)
            b[i] = malloc(size * sizeof(int));
        int *result = calloc(size, sizeof(int));

        if (fill_choice == 1)
        {
            printf("Enter vector elements:\n");
            for (int i = 0; i < size; i++)
            {
                printf("a[%d]: ", i);
                scanf("%d", &a[i]);
            }

            printf("Enter matrix elements:\n");
            for (int i = 0; i < size; i++)
                for (int j = 0; j < size; j++)
                {
                    printf("b[%d][%d]: ", i, j);
                    scanf("%d", &b[i][j]);
                }
        }
        else
        {
            srand(0);
            printf("Random vector:\n");
            for (int i = 0; i < size; i++)
            {
                a[i] = rand() % 10;
                printf("%d ", a[i]);
            }
            printf("\nRandom matrix:\n");
            for (int i = 0; i < size; i++)
            {
                for (int j = 0; j < size; j++)
                {
                    b[i][j] = rand() % 10;
                    printf("%d ", b[i][j]);
                }
                printf("\n");
            }
        }

        double start_time = get_time();

#pragma omp parallel for default(none) shared(a, b, result, size)
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                result[i] += a[j] * b[j][i];
            }
        }

        double end_time = get_time();

        printf("\nResult vector:\n");
        for (int i = 0; i < size; i++)
            printf("%d ", result[i]);
        printf("\nTime taken for matrix-vector multiplication: %.3f ms\n", (end_time - start_time) * 1000.0);

        free(a);
        for (int i = 0; i < size; i++)
            free(b[i]);
        free(b);
        free(result);
    }
    else
    {
        printf("Invalid choice.\n");
        return 1;
    }

    return 0;
}
