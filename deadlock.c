
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size;
    int send_data = 100;
    int recv_data = 0;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        printf("Rank 0: Sending synchronously to Rank 1...\n");
        fflush(stdout);

        MPI_Ssend(&send_data, 1, MPI_INT,
                  1, 0, MPI_COMM_WORLD);

        MPI_Recv(&recv_data, 1, MPI_INT,
                 1, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }
    else {
        printf("Rank 1: Sending synchronously to Rank 0...\n");
        fflush(stdout);

        MPI_Ssend(&send_data, 1, MPI_INT,
                  0, 0, MPI_COMM_WORLD);

        MPI_Recv(&recv_data, 1, MPI_INT,
                 0, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }

    printf("Rank %d: Received %d\n", rank, recv_data);

    MPI_Finalize();
    return 0;
}

