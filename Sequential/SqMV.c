#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

double get_time()
{
#ifdef _WIN32
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / frequency.QuadPart;
#else
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
#endif
}

int main()
{
    printf("Select method:\n");
    printf("1 : Solving a system of equations.\n");
    printf("2 : Matrix multiplication.\n");
    int choice;
    scanf("%d", &choice);

    if (choice == 1)
    {
        printf("You chose: System of equations.\n");
        printf("Enter number of variables: ");
        int n;
        scanf("%d", &n);

        int **coeff = malloc(n * sizeof(int *));
        for (int i = 0; i < n; i++)
            coeff[i] = malloc(n * sizeof(int));
        int *constants = malloc(n * sizeof(int));

        printf("Enter coefficients and constant term for each equation:\n");
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

        double start_time = get_time();

        double **L = malloc(n * sizeof(double *));
        double **U = malloc(n * sizeof(double *));
        for (int i = 0; i < n; i++)
        {
            L[i] = calloc(n, sizeof(double));
            U[i] = calloc(n, sizeof(double));
        }

        for (int i = 0; i < n; i++)
        {
            for (int j = i; j < n; j++)
            {
                double sum = 0;
                for (int k = 0; k < i; k++)
                    sum += L[i][k] * U[k][j];
                U[i][j] = A[i][j] - sum;
            }

            L[i][i] = 1.0;

            for (int j = i + 1; j < n; j++)
            {
                double sum = 0;
                for (int k = 0; k < i; k++)
                    sum += L[j][k] * U[k][i];
                if (U[i][i] == 0)
                {
                    printf("Error: Division by zero in LU factorization.\n");
                    exit(1);
                }
                L[j][i] = (A[j][i] - sum) / U[i][i];
            }
        }

        double *y = malloc(n * sizeof(double));
        for (int i = 0; i < n; i++)
        {
            double sum = 0;
            for (int j = 0; j < i; j++)
                sum += L[i][j] * y[j];
            y[i] = b[i] - sum;
        }

        double *x = malloc(n * sizeof(double));
        for (int i = n - 1; i >= 0; i--)
        {
            double sum = 0;
            for (int j = i + 1; j < n; j++)
                sum += U[i][j] * x[j];
            if (U[i][i] == 0)
            {
                printf("Error: Division by zero in backward substitution.\n");
                exit(1);
            }
            x[i] = (y[i] - sum) / U[i][i];
        }

        double end_time = get_time();

        printf("\nSolution:\n");
        for (int i = 0; i < n; i++)
            printf("Variable %d = %.3lf\n", i + 1, x[i]);
        printf("Time taken for LU decomposition and solving: %.3f milliseconds\n", (end_time - start_time) * 1000.0);

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
        printf("You chose: Matrix multiplication.\n");
        printf("1: Manual fill\n2: Auto random fill\n");
        int fill_option;
        scanf("%d", &fill_option);

        printf("Enter size of vector/matrix: ");
        int n;
        scanf("%d", &n);

        int *a = malloc(n * sizeof(int));
        int **b = malloc(n * sizeof(int *));
        for (int i = 0; i < n; i++)
            b[i] = malloc(n * sizeof(int));
        int *result = calloc(n, sizeof(int));

        if (fill_option == 1)
        {
            printf("Enter vector elements:\n");
            for (int i = 0; i < n; i++)
            {
                printf("a[%d]: ", i);
                scanf("%d", &a[i]);
            }

            printf("Enter matrix elements:\n");
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                {
                    printf("b[%d][%d]: ", i, j);
                    scanf("%d", &b[i][j]);
                }
        }
        else
        {
            srand(0);
            printf("Random vector:\n");
            for (int i = 0; i < n; i++)
            {
                a[i] = rand() % 10;
                printf("%d ", a[i]);
            }
            printf("\nRandom matrix:\n");
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    b[i][j] = rand() % 10;
                    printf("%d ", b[i][j]);
                }
                printf("\n");
            }
        }

        double start_time = get_time();

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                result[i] += a[j] * b[j][i];

        double end_time = get_time();

        printf("Result vector:\n");
        for (int i = 0; i < n; i++)
            printf("%d ", result[i]);
        printf("\nTime taken for matrix-vector multiplication: %.3f milliseconds\n", (end_time - start_time) * 1000.0);

        free(a);
        for (int i = 0; i < n; i++)
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
