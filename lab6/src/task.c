#include "../../task.h"

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

int task(int world_size, int world_rank, int n_args, char* args[])
{
    matrix_t A = {0}, B = {0}, C = {0};

    /*
    Amount of elements which this process will recieve
    This var necessary to store sendcounts scatterv result
    */
    int recvcount = 0;

    int* sendcounts = NULL;
    int* displs = NULL;

    if (world_rank == 0) {
        // Get matrices
        if (n_args == 3) {
            int n = atoi(args[1]);
            int m = atoi(args[2]);
            if (n <= 0 || m <= 0) {
                MPI_Abort(MPI_COMM_WORLD, 1);
                return 1;
            }
            matrix_gen(&A, n, m);
            matrix_gen(&B, n, m);
        } else {
            matrix_read(&A, stdin);
            matrix_read(&B, stdin);
        }

        if (A.m != B.n) {
            fprintf(stderr, "Matrices cannot be multiplied\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        matrix_init(A.n, B.m, &C);

        sendcounts = malloc(world_size * sizeof(int));
        displs     = malloc(world_size * sizeof(int));
        memset(sendcounts, 0, world_size * sizeof(int));
        memset(displs, 0, world_size * sizeof(int));

        // Distribute rows of A
        int estimation = A.n / world_size;
        int overhead   = A.n % world_size;
        int row        = 0;

        for (int i = 0; i < world_size; ++i) {
            int rows = estimation + (i < overhead ? 1 : 0);
            sendcounts[i] = rows * A.m;
            displs[i]     = row * A.m;
            row += rows;
        }
    }

    // Broadcast matrix sizes
    MPI_Bcast(&A.n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&A.m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&B.n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&B.m, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Scatter a row counts
    MPI_Scatter(sendcounts, 1, MPI_INT,
                &recvcount, 1, MPI_INT,
                0, MPI_COMM_WORLD);

    int local_rows = recvcount / A.m;

    // Recieve local A
    matrix_t A_local;
    matrix_init(local_rows, A.m, &A_local);

    MPI_Scatterv(
        A.data, sendcounts, displs, MPI_INT,
        A_local.data, recvcount, MPI_INT,
        0, MPI_COMM_WORLD
    );

    // Broadcast B
    if (world_rank != 0) {
        matrix_init(B.n, B.m, &B);
    }

    MPI_Bcast(B.data, B.n * B.m, MPI_INT, 0, MPI_COMM_WORLD);

    // Local multiplication
    matrix_t C_local;
    matrix_init(local_rows, B.m, &C_local);
    memset(C_local.data, 0, local_rows * B.m * sizeof(int));

    matrix_multiply(&A_local, &B, &C_local);

    // Gather results
    int* recvcounts_C = NULL;
    int* displs_C = NULL;

    if (world_rank == 0) {
        recvcounts_C = malloc(world_size * sizeof(int));
        displs_C     = malloc(world_size * sizeof(int));

        for (int i = 0; i < world_size; ++i) {
            recvcounts_C[i] = sendcounts[i] / A.m * B.m;
            displs_C[i]     = displs[i] / A.m * B.m;
        }
    }

    MPI_Gatherv(
        C_local.data, local_rows * B.m, MPI_INT,
        C.data, recvcounts_C, displs_C, MPI_INT,
        0, MPI_COMM_WORLD
    );

#ifndef MEASURE_TIME
    // Print result
    if (world_rank == 0) {
        matrix_print(&C);
    }
#endif

    // Perform cleanup
    matrix_free(&A_local);
    matrix_free(&C_local);
    matrix_free(&B);

    if (world_rank == 0) {
        matrix_free(&A);
        matrix_free(&C);
        free(sendcounts);
        free(displs);
        free(recvcounts_C);
        free(displs_C);
    }

    return 0;
}
