
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size;

    // Initialize MPI environment
    MPI_Init(&argc, &argv);

    // Get the rank of current process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the total number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Print message from each process
    printf("Hello from rank %d of %d\n", rank, size);

    // Wait for all processes to finish
    MPI_Barrier(MPI_COMM_WORLD);

    // Terminate MPI environment
    MPI_Finalize();

    return 0;
}
