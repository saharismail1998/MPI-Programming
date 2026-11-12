
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ITERATIONS 100

int main(int argc, char **argv)
{
    int rank, size;

    int message_sizes[] = {
        1024,
        65536,
        1048576,
        4194304
    };

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0)
            printf("Run with exactly 2 processes.\n");

        MPI_Finalize();
        return 1;
    }

    for (int s = 0; s < 4; s++) {

        int n = message_sizes[s];

        char *sendbuf = malloc(n);
        char *recvbuf = malloc(n);

        if (!sendbuf || !recvbuf)
            MPI_Abort(MPI_COMM_WORLD, 1);

        for (int i = 0; i < n; i++)
            sendbuf[i] = 'A';

        double total = 0.0;

        MPI_Barrier(MPI_COMM_WORLD);

        for (int i = 0; i < ITERATIONS; i++) {

            if (rank == 0) {

                double start = MPI_Wtime();

                MPI_Send(sendbuf, n, MPI_BYTE,
                         1, 0, MPI_COMM_WORLD);

                MPI_Recv(recvbuf, n, MPI_BYTE,
                         1, 0, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);

                total += MPI_Wtime() - start;
            }
            else {

                MPI_Recv(recvbuf, n, MPI_BYTE,
                         0, 0, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);

                MPI_Send(sendbuf, n, MPI_BYTE,
                         0, 0, MPI_COMM_WORLD);
            }
        }

        if (rank == 0) {

            double rtt = total / ITERATIONS;
            double bandwidth = n / (rtt / 2.0);

            printf("Blocking | Size: %d bytes | "
                   "RTT: %.9f sec | Bandwidth: %.3f MB/s\n",
                   n, rtt, bandwidth / (1024.0 * 1024.0));
        }

        total = 0.0;

        MPI_Barrier(MPI_COMM_WORLD);

        for (int i = 0; i < ITERATIONS; i++) {

            if (rank == 0) {

                double start = MPI_Wtime();

                MPI_Request req[2];

                MPI_Isend(sendbuf, n, MPI_BYTE,
                          1, 1, MPI_COMM_WORLD, &req[0]);

                MPI_Irecv(recvbuf, n, MPI_BYTE,
                          1, 1, MPI_COMM_WORLD, &req[1]);

                MPI_Waitall(2, req, MPI_STATUSES_IGNORE);

                total += MPI_Wtime() - start;
            }
            else {

                MPI_Request req[2];

                MPI_Irecv(recvbuf, n, MPI_BYTE,
                          0, 1, MPI_COMM_WORLD, &req[0]);

                MPI_Isend(sendbuf, n, MPI_BYTE,
                          0, 1, MPI_COMM_WORLD, &req[1]);

                MPI_Waitall(2, req, MPI_STATUSES_IGNORE);
            }
        }

        if (rank == 0) {

            double rtt = total / ITERATIONS;
            double bandwidth = n / (rtt / 2.0);

            printf("Nonblocking | Size: %d bytes | "
                   "RTT: %.9f sec | Bandwidth: %.3f MB/s\n\n",
                   n, rtt, bandwidth / (1024.0 * 1024.0));
        }

        free(sendbuf);
        free(recvbuf);
    }

    MPI_Finalize();
    return 0;
}

