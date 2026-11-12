
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define N 2048

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0) {
        if (rank == 0)
            printf("N must be divisible by number of processes.\n");
        MPI_Finalize();
        return 1;
    }

    int rows = N / size;

    double *A = NULL;
    double *C = NULL;
    double *B = malloc((size_t)N * N * sizeof(double));

    double *local_A =
        malloc((size_t)rows * N * sizeof(double));

    double *local_C =
        calloc((size_t)rows * N, sizeof(double));

    if (!B || !local_A || !local_C)
        MPI_Abort(MPI_COMM_WORLD, 1);

    if (rank == 0) {
        A = malloc((size_t)N * N * sizeof(double));
        C = malloc((size_t)N * N * sizeof(double));

        if (!A || !C)
            MPI_Abort(MPI_COMM_WORLD, 1);

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i * N + j] = (double)((i + j) % 100) / 100.0;
                B[i * N + j] = (double)((i - j + N) % 100) / 100.0;
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double total_start = MPI_Wtime();

    /* Broadcast matrix B */
    MPI_Bcast(B, N * N, MPI_DOUBLE,
              0, MPI_COMM_WORLD);

    /* Scatter rows of matrix A */
    MPI_Scatter(A, rows * N, MPI_DOUBLE,
                local_A, rows * N, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    double compute_start = MPI_Wtime();

    /* Local matrix multiplication */
    for (int i = 0; i < rows; i++) {
        for (int k = 0; k < N; k++) {

            double a = local_A[i * N + k];

            for (int j = 0; j < N; j++) {
                local_C[i * N + j] +=
                    a * B[k * N + j];
            }
        }
    }

    double compute_time = MPI_Wtime() - compute_start;

    /* Gather results */
    MPI_Gather(local_C, rows * N, MPI_DOUBLE,
               C, rows * N, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    double total_time = MPI_Wtime() - total_start;

    double max_total, max_compute;

    MPI_Reduce(&total_time, &max_total, 1,
               MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    MPI_Reduce(&compute_time, &max_compute, 1,
               MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {

        /* Serial reference verification */
        double *reference =
            malloc((size_t)N * N * sizeof(double));

        if (!reference)
            MPI_Abort(MPI_COMM_WORLD, 1);

        for (int i = 0; i < N * N; i++)
            reference[i] = 0.0;

        double ref_start = MPI_Wtime();

        for (int i = 0; i < N; i++) {
            for (int k = 0; k < N; k++) {

                double a = A[i * N + k];

                for (int j = 0; j < N; j++) {
                    reference[i * N + j] +=
                        a * B[k * N + j];
                }
            }
        }

        double ref_time = MPI_Wtime() - ref_start;

        int correct = 1;

        for (int i = 0; i < N * N; i++) {
            if (fabs(C[i] - reference[i]) > 1e-8) {
                correct = 0;
                break;
            }
        }

        double comm_time = max_total - max_compute;

        printf("\nProcesses: %d\n", size);
        printf("Matrix Size: %d x %d\n", N, N);
        printf("MPI Runtime: %.6f seconds\n", max_total);
        printf("Compute Time: %.6f seconds\n", max_compute);
        printf("Communication Overhead: %.6f seconds\n",
               comm_time);
        printf("Serial Reference Time: %.6f seconds\n", ref_time);

        printf("Verification: %s\n",
               correct ? "PASSED" : "FAILED");

        printf("CSV,%d,%.9f,%.9f,%.9f\n",
               size, max_total, max_compute, comm_time);

        free(reference);
    }

    free(B);
    free(local_A);
    free(local_C);

    if (rank == 0) {
        free(A);
        free(C);
    }

    MPI_Finalize();

    return 0;
}

