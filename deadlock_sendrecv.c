
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

    MPI_Sendrecv(
        &send_data, 1, MPI_INT,
        1 - rank, 0,
        &recv_data, 1, MPI_INT,
        1 - rank, 0,
        MPI_COMM_WORLD,
        MPI_STATUS_IGNORE
    );

    printf("Rank %d received: %d\n",
           rank, recv_data);

    MPI_Finalize();
    return 0;
}

