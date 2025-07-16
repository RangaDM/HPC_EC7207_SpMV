#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void lu_decomposition(int n, double **A, double *b, double *x, int rank, int size)
{
    double **L = (double **)malloc(n * sizeof(double *));
    double **U = (double **)malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++)
    {
        L[i] = calloc(n, sizeof(double));
        U[i] = calloc(n, sizeof(double));
    }

    if (rank == 0)
    {
        // LU Decomposition - Serial on Master (for simplicity)
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
                L[j][i] = (A[j][i] - sum) / U[i][i];
            }
        }

        // Forward substitution Ly = b
        double *y = malloc(n * sizeof(double));
        for (int i = 0; i < n; i++)
        {
            double sum = 0;
            for (int j = 0; j < i; j++)
                sum += L[i][j] * y[j];
            y[i] = b[i] - sum;
        }

        // Backward substitution Ux = y
        for (int i = n - 1; i >= 0; i--)
        {
            double sum = 0;
            for (int j = i + 1; j < n; j++)
                sum += U[i][j] * x[j];
            x[i] = (y[i] - sum) / U[i][i];
        }

        free(y);
    }

    for (int i = 0; i < n; i++)
    {
        free(L[i]);
        free(U[i]);
    }
    free(L);
    free(U);
}

void matrix_vector_mult(int size, int *a, int **b, int *result, int rank, int comm_size)
{
    int rows_per_proc = size / comm_size;
    int extra = size % comm_size;
    int *sendcounts = malloc(comm_size * sizeof(int));
    int *displs = malloc(comm_size * sizeof(int));
    int offset = 0;

    for (int i = 0; i < comm_size; i++)
    {
        sendcounts[i] = rows_per_proc;
        if (i < extra)
            sendcounts[i]++;
        displs[i] = offset;
        offset += sendcounts[i];
    }

    int recv_rows = sendcounts[rank];
    int *local_result = calloc(recv_rows, sizeof(int));
    int **local_b = malloc(recv_rows * sizeof(int *));
    for (int i = 0; i < recv_rows; i++)
    {
        local_b[i] = malloc(size * sizeof(int));
    }

    for (int i = 0; i < recv_rows; i++)
    {
        MPI_Scatterv(&(b[0][0]), sendcounts, displs, MPI_INT,
                     local_b[i], size, MPI_INT, 0, MPI_COMM_WORLD);
    }

    for (int i = 0; i < recv_rows; i++)
    {
        for (int j = 0; j < size; j++)
            local_result[i] += a[j] * local_b[i][j];
    }

    MPI_Gatherv(local_result, recv_rows, MPI_INT,
                result, sendcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < recv_rows; i++)
        free(local_b[i]);
    free(local_b);
    free(local_result);
    free(sendcounts);
    free(displs);
}

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        printf("Select method:\n");
        printf("1 : Solving a system of equations.\n");
        printf("2 : Matrix multiplication.\n");
    }

    int choice;
    if (rank == 0)
        scanf("%d", &choice);

    MPI_Bcast(&choice, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (choice == 1)
    {
        int n;
        if (rank == 0)
        {
            printf("You chose: System of equations.\nEnter number of variables: ");
            scanf("%d", &n);
        }
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

        double **A = (double **)malloc(n * sizeof(double *));
        double *b = (double *)malloc(n * sizeof(double));
        double *x = (double *)malloc(n * sizeof(double));

        if (rank == 0)
        {
            for (int i = 0; i < n; i++)
            {
                A[i] = (double *)malloc(n * sizeof(double));
                printf("Equation %d coefficients:\n", i + 1);
                for (int j = 0; j < n; j++)
                {
                    scanf("%lf", &A[i][j]);
                }
                printf("Constant term: ");
                scanf("%lf", &b[i]);
            }
        }

        double start_time = MPI_Wtime();
        lu_decomposition(n, A, b, x, rank, size);
        double end_time = MPI_Wtime();

        if (rank == 0)
        {
            printf("Solution:\n");
            for (int i = 0; i < n; i++)
                printf("Variable %d = %.3lf\n", i + 1, x[i]);
            printf("Time taken: %.3f ms\n", (end_time - start_time) * 1000.0);
        }

        if (rank == 0)
        {
            for (int i = 0; i < n; i++)
                free(A[i]);
        }
        free(A);
        free(b);
        free(x);
    }
    else if (choice == 2)
    {
        int fill_type;
        if (rank == 0)
        {
            printf("You chose: Matrix multiplication.\n1: Manual\n2: Auto Random\n");
            scanf("%d", &fill_type);
        }
        MPI_Bcast(&fill_type, 1, MPI_INT, 0, MPI_COMM_WORLD);

        int n;
        if (rank == 0)
        {
            printf("Enter size of matrix/vector: ");
            scanf("%d", &n);
        }
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

        int *a = (int *)malloc(n * sizeof(int));
        int **b = (int **)malloc(n * sizeof(int *));
        int *result = (int *)calloc(n, sizeof(int));
        for (int i = 0; i < n; i++)
            b[i] = (int *)malloc(n * sizeof(int));

        if (rank == 0)
        {
            if (fill_type == 1)
            {
                printf("Enter vector elements:\n");
                for (int i = 0; i < n; i++)
                    scanf("%d", &a[i]);

                printf("Enter matrix elements:\n");
                for (int i = 0; i < n; i++)
                    for (int j = 0; j < n; j++)
                        scanf("%d", &b[i][j]);
            }
            else
            {
                srand(0);
                for (int i = 0; i < n; i++)
                    a[i] = rand() % 10;
                for (int i = 0; i < n; i++)
                    for (int j = 0; j < n; j++)
                        b[i][j] = rand() % 10;
            }
        }

        double start_time = MPI_Wtime();
        matrix_vector_mult(n, a, b, result, rank, size);
        double end_time = MPI_Wtime();

        if (rank == 0)
        {
            printf("Result vector:\n");
            for (int i = 0; i < n; i++)
                printf("%d ", result[i]);
            printf("\nTime taken: %.3f ms\n", (end_time - start_time) * 1000.0);
        }

        free(a);
        for (int i = 0; i < n; i++)
            free(b[i]);
        free(b);
        free(result);
    }

    MPI_Finalize();
    return 0;
}
