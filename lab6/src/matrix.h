#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int   n;
    int   m;
    int* data;
} matrix_t;

int  matrix_init     (int n, int m, matrix_t* matrix);
int  matrix_read     (matrix_t* matrix, FILE* stream);
int  matrix_multiply (const matrix_t* A, const matrix_t* B, matrix_t* C);
void matrix_print    (const matrix_t* matrix);
void matrix_free     (matrix_t* matrix);

#endif  // MATRIX_H
