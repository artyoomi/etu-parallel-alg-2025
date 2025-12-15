#include <stdio.h>

#include "matrix.h"

#include "../../task.h"

int task(int world_size, int world_rank, int n_args, char* args[])
{
    matrix_t A, B, C;

    matrix_read(&A, stdin);
    matrix_read(&B, stdin);
    matrix_init(A.n, B.m, &C);

    matrix_multiply(&A, &B, &C);
    matrix_print(&C);

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&C);

    return 0;
}
