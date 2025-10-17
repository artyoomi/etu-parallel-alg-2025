#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

typedef struct {
    int max_value;
    double elapsed_time;
    int retval;
} computation_result_t;

int utils_generate_array(int* array, size_t array_size);
void utils_print_array(const int* array, size_t array_size);
int utils_error(int error, char* buf, size_t buf_size);

// Main function to find max using any compatible program
computation_result_t utils_max(int* array, size_t array_size,
                               const char* prog_name, char* const args[]);

#endif // ARRAY_UTILS_H
