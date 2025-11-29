#include <stdio.h>

#include "matrix.h"

int task(int world_size, int world_rank, int n_args, char* args[])
{
    // Get m
    int m = 0;
    if (!scanf("%d", &m) || m <= 0) {
        fprintf(stderr, "Invalid matrix width value");
        return 1;
    }

    matrix_t A, B, C;
    matrix_init(m, m, &A);
    matrix_init(m, m, &B);
    matrix_init(m, m, &C);

    matrix_read(&A, stdin);
    matrix_read(&B, stdin);

    matrix_multiply(&A, &B, &C);
    matrix_print(&C);

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&C);

    return 0;
}
