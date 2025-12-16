#include <stdio.h>

#include "matrix.h"

#include "../../task.h"

int task(int world_size, int world_rank, int n_args, char* args[])
{
    matrix_t A, B, C;

    if (world_size != 1) {
        if (world_rank == 0) {
            fprintf(stderr,
                    "This program cannot be run with two and more processes\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

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

    matrix_init(A.n, B.m, &C);

    matrix_multiply(&A, &B, &C);

#ifndef MEASURE_TIME
    matrix_print(&C);
#endif

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&C);

    return 0;
}
