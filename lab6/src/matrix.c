#include "matrix.h"

int matrix_init(int n, int m, matrix_t* matrix)
{
    matrix->n = n;
    matrix->m = m;

    matrix->data = (int*)malloc(n * m * sizeof(int*));
    if (!matrix->data) { return 1; }

    return 0;
}

int matrix_read(matrix_t* matrix, FILE* stream)
{
    int n = 0, m = 0;
    if (!fscanf(stream, "%d", &n) || n <= 0) {
        fprintf(stderr, "Invalid matrix rows count value");
        return 1;
    }
    if (!fscanf(stream, "%d", &m) || m <= 0) {
        fprintf(stderr, "Invalid matrix cols count value");
        return 1;
    }
    matrix_init(n, m, matrix);

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            if (!fscanf(stream, "%d", &(matrix->data[i * m + j]))) {
                return 1;
            }
        }
    }

    return 0;
}

int matrix_multiply(const matrix_t* A, const matrix_t* B, matrix_t* C)
{
    if (A->m != B->n) {
        fprintf(stderr, "A and B cannot be multiplied");
        return 1;
    }

    for (size_t i = 0; i < A->n; ++i) {
        for (size_t j = 0; j < B->m; ++j) {
            for (size_t k = 0; k < A->m; ++k) {
                C->data[i * B->m + j] += A->data[i * A->m + k] * B->data[k * B->m + j];
            }
        }
    }

    return 0;
}

void matrix_print(const matrix_t* matrix)
{
    for (size_t i = 0; i < matrix->n; ++i) {
        for (size_t j = 0; j < matrix->m; ++j) {
            printf("%d", matrix->data[i * matrix->m + j]);
            if (j == matrix->m - 1) { putchar('\n'); }
            else { putchar(' '); }
        }
    }
}

void matrix_free(matrix_t* matrix)
{
    free(matrix->data);

    matrix->n = 0;
    matrix->m = 0;
}
