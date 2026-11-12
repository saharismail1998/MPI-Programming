
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size;
    int send_data = 100, recv_data = 0;

    MPI_Request requests[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        MPI_Finalize();
        return 1;
    }

    // Start receiving
    MPI_Irecv(
        &recv_data, 1, MPI_INT,
        1 - rank, 0,
        MPI_COMM_WORLD,
        &requests[0]
    );

    // Start sending
    MPI_Isend(
        &send_data, 1, MPI_INT,
        1 - rank, 0,
        MPI_COMM_WORLD,
        &requests[1]
    );

    // Wait for both operations
    MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);

    printf("Rank %d received: %d\n",
           rank, recv_data);

    MPI_Finalize();
    return 0;
}

