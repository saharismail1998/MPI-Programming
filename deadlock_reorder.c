
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size;
    int send_data = 100, recv_data = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        printf("Rank 0: Sending to Rank 1\n");

        MPI_Send(&send_data, 1, MPI_INT,
                 1, 0, MPI_COMM_WORLD);

        MPI_Recv(&recv_data, 1, MPI_INT,
                 1, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }
    else {
        printf("Rank 1: Receiving from Rank 0\n");

        MPI_Recv(&recv_data, 1, MPI_INT,
                 0, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        MPI_Send(&send_data, 1, MPI_INT,
                 0, 0, MPI_COMM_WORLD);
    }

    printf("Rank %d received: %d\n", rank, recv_data);

    MPI_Finalize();
    return 0;
}

