#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
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

int main(int argc, char *argv[])
{
    int rank, size_mpi;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size_mpi);

    if (rank == 0)
        printf("Hybrid MPI + OpenMP Program Running with %d MPI processes\n", size_mpi);

    int method_choice;
    if (rank == 0)
    {
        printf("Select method:\n1: System of Equations\n2: Matrix Multiplication\n");
        scanf("%d", &method_choice);
    }

    MPI_Bcast(&method_choice, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (method_choice == 2)
    {
        int fill_choice, matrix_size;
        if (rank == 0)
        {
            printf("You chose: Matrix Multiplication (Hybrid)\n");
            printf("1: Manual fill\n2: Auto random fill\n");
            scanf("%d", &fill_choice);

            printf("Enter size of vector/matrix: ");
            scanf("%d", &matrix_size);
        }

        MPI_Bcast(&fill_choice, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&matrix_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

        int *vector = malloc(matrix_size * sizeof(int));
        int **matrix = malloc(matrix_size * sizeof(int *));
        int *result = calloc(matrix_size, sizeof(int));

        for (int i = 0; i < matrix_size; i++)
            matrix[i] = malloc(matrix_size * sizeof(int));

        if (rank == 0)
        {
            if (fill_choice == 1)
            {
                printf("Enter vector elements:\n");
                for (int i = 0; i < matrix_size; i++)
                {
                    printf("a[%d]: ", i);
                    scanf("%d", &vector[i]);
                }

                printf("Enter matrix elements:\n");
                for (int i = 0; i < matrix_size; i++)
                    for (int j = 0; j < matrix_size; j++)
                    {
                        printf("b[%d][%d]: ", i, j);
                        scanf("%d", &matrix[i][j]);
                    }
            }
            else
            {
                srand(0);
                printf("Random vector:\n");
                for (int i = 0; i < matrix_size; i++)
                {
                    vector[i] = rand() % 10;
                    printf("%d ", vector[i]);
                }
                printf("\nRandom matrix:\n");
                for (int i = 0; i < matrix_size; i++)
                {
                    for (int j = 0; j < matrix_size; j++)
                    {
                        matrix[i][j] = rand() % 10;
                        printf("%d ", matrix[i][j]);
                    }
                    printf("\n");
                }
            }
        }

        // Broadcast vector to all
        MPI_Bcast(vector, matrix_size, MPI_INT, 0, MPI_COMM_WORLD);

        // Scatter rows of matrix to each process
        int rows_per_proc = matrix_size / size_mpi;
        int remainder = matrix_size % size_mpi;
        int start_row = rank * rows_per_proc + (rank < remainder ? rank : remainder);
        int num_rows = rows_per_proc + (rank < remainder ? 1 : 0);

        int **local_matrix = malloc(num_rows * sizeof(int *));
        for (int i = 0; i < num_rows; i++)
            local_matrix[i] = malloc(matrix_size * sizeof(int));

        if (rank == 0)
        {
            int offset = 0;
            for (int p = 0; p < size_mpi; p++)
            {
                int send_rows = rows_per_proc + (p < remainder ? 1 : 0);
                if (p == 0)
                {
                    for (int i = 0; i < send_rows; i++)
                        for (int j = 0; j < matrix_size; j++)
                            local_matrix[i][j] = matrix[i][j];
                }
                else
                {
                    for (int i = 0; i < send_rows; i++)
                        MPI_Send(matrix[offset + i], matrix_size, MPI_INT, p, 0, MPI_COMM_WORLD);
                }
                offset += send_rows;
            }
        }
        else
        {
            for (int i = 0; i < num_rows; i++)
                MPI_Recv(local_matrix[i], matrix_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        int *local_result = calloc(matrix_size, sizeof(int));

        double start_time = get_time();

#pragma omp parallel for default(none) shared(local_matrix, vector, local_result, num_rows, matrix_size, start_row) collapse(2)
        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < matrix_size; j++)
            {
                local_result[j] += local_matrix[i][j] * vector[start_row + i];
            }
        }

        double end_time = get_time();
        double local_elapsed = (end_time - start_time) * 1000.0;

        if (rank == 0)
        {
            for (int i = 0; i < matrix_size; i++)
                result[i] = 0;
        }

        MPI_Reduce(local_result, result, matrix_size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0)
        {
            printf("\nResult vector:\n");
            for (int i = 0; i < matrix_size; i++)
                printf("%d ", result[i]);
            printf("\n");
        }

        printf("Rank %d completed in %.3f ms\n", rank, local_elapsed);

        for (int i = 0; i < matrix_size; i++)
            free(matrix[i]);
        free(matrix);
        free(vector);
        free(result);

        for (int i = 0; i < num_rows; i++)
            free(local_matrix[i]);
        free(local_matrix);
        free(local_result);
    }
    else
    {
        if (rank == 0)
            printf("Only Matrix Multiplication implemented in Hybrid.\n");
    }

    MPI_Finalize();
    return 0;
}
