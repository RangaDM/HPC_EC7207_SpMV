#include <stdio.h>
#include <stdlib.h>
#include <omp.h> // Include OpenMP header for parallel processing
#ifdef _WIN32    // Check if the OS is Windows
#include <windows.h>
#else
#include <time.h>
#include <sys/time.h>
#endif

// Function to get current time in seconds
double get_time()
{
#ifdef _WIN32
    LARGE_INTEGER frequency, start;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    return (double)start.QuadPart / frequency.QuadPart;
#else
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    return start.tv_sec + start.tv_nsec / 1e9;
#endif
}

int main()
{
    printf("Select method:\n");
    printf("1 : Solving a system of equations.\n");
    printf("2 : Matrix multiplication.\n");
    int choice1;
    scanf("%d", &choice1);

    // omp_set_num_threads(5); // Set number of Threads at Runtime

    // ************* Part 1 : Solving a system of equations *************
    if (choice1 == 1) // Chose to solve a system of equations
    {
        printf("You chose: System of equations.\n");
        printf("Enter the number of equations (variables): ");
        int n;
        scanf("%d", &n);

        // Dynamically allocate memory for the coefficient matrix and constants vector
        int **coeff = (int **)malloc(n * sizeof(int *));
        for (int i = 0; i < n; i++)
        {
            coeff[i] = (int *)malloc(n * sizeof(int));
        }
        int *constants = (int *)malloc(n * sizeof(int)); // Constants vector

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

            double start_time = get_time();

            // Allocate memory for L and U matrices
            double **L = malloc(n * sizeof(double *));
            double **U = malloc(n * sizeof(double *));
            for (int i = 0; i < n; i++)
            {
                L[i] = calloc(n, sizeof(double));
                U[i] = calloc(n, sizeof(double));
            }

            // Parallelize LU Factorization
            for (int i = 0; i < n; i++)
            {
#pragma omp parallel for shared(U, L, A, i, n) default(none)
                // Compute U's row (parallel over j)
                for (int j = i; j < n; j++)
                {
                    double sum = 0;
                    for (int k = 0; k < i; k++)
                    {
                        sum += L[i][k] * U[k][j];
                    }
                    U[i][j] = A[i][j] - sum;
                }

                L[i][i] = 1.0;

#pragma omp parallel for shared(L, U, A, i, n) default(none)
                // Compute L's column (parallel over j)
                for (int j = i + 1; j < n; j++)
                {
                    double sum = 0;
                    for (int k = 0; k < i; k++)
                    {
                        sum += L[j][k] * U[k][i];
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
#pragma omp parallel for reduction(+ : sum) shared(L, y, i) default(none)
                for (int j = 0; j < i; j++)
                {
                    sum += L[i][j] * y[j];
                }
                y[i] = b[i] - sum;
            }

            // Backward substitution: solve Ux = y
            double *x = malloc(n * sizeof(double));
            // Parallelize Forward and Backward Substitution
            for (int i = n - 1; i >= 0; i--)
            {
                double sum = 0;
#pragma omp parallel for reduction(+ : sum) default(none) shared(i, x, U, n)
                for (int j = i + 1; j < n; j++)
                {
                    sum += U[i][j] * x[j];
                }
                x[i] = (y[i] - sum) / U[i][i];
            }

            // Print the solution
            printf("\nSolution:\n");
            for (int i = 0; i < n; i++)
            {
                printf("Variable %d: %lf\n", i + 1, x[i]);
            }

            double end_time = get_time();
            double execution_time = (end_time - start_time) * 1000.0; // Convert to milliseconds
            printf("\nTime taken for the operation: %.3f milliseconds\n", execution_time);

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

    // ************* Part 2 : Matrix multiplication *************
    else if (choice1 == 2) // Matrix multiplication
    {
        printf("You chose: Matrix multiplication.\n\n");
        printf("Select method:\n");
        printf("1 : Manually fill.\n");
        printf("2 : Automatically fill.\n");
        int n;
        scanf("%d", &n);

        // ************* Part 2.1 : Manually fill *************
        if (n == 1) // Manually fill
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

            double start_time = get_time();

// Vector-matrix multiplication: result = a * b
#pragma omp parallel for private(j) shared(a, b, result, size) default(none)
            for (int i = 0; i < size; i++)
            {
                for (int j = 0; j < size; j++)
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

            double end_time = get_time();
            double execution_time = (end_time - start_time) * 1000.0; // Convert to milliseconds
            printf("\nTime taken for the operation: %.3f milliseconds\n", execution_time);

            // Free memory
            free(a);
            for (i = 0; i < size; i++)
            {
                free(b[i]);
            }
            free(b);
            free(result);
        }

        // ************* Part 2.2 : Automatically fill *************
        else if (n == 2) // Automatically fill
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
            printf("\nVector a (randomly filled):\n");
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

            double start_time = get_time();

            // Vector-matrix multiplication: result = a * b
            for (i = 0; i < size; i++)
            {
                for (j = 0; j < size; j++)
                {
                    result[i] += a[j] * b[j][i];
                }
            }

            printf("\nResultant vector (a * b) : ");
            for (i = 0; i < size; i++)
            {
                printf("%d  ", result[i]);
            }
            printf("\n");

            double end_time = get_time();
            double execution_time = (end_time - start_time) * 1000.0; // Convert to milliseconds
            printf("\nTime taken for the operation: %.3f milliseconds\n", execution_time);

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

    return 0;
}

// Compile with: gcc -fopenmp -o SqMV_OpenMP SqMV_OpenMP.c