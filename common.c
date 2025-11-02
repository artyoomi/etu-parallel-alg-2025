#include "common.h"

void generate_array(int* buf, size_t bufsize, unsigned int seed)
{
    if (buf == NULL) return;
    
    srand(seed);

    const int upper_bound = 1000;
    for (size_t i = 0; i < bufsize; ++i) {
        buf[i] = rand() % upper_bound;
    }
}

void print_array(int* buf, size_t bufsize)
{
    if (buf == NULL || bufsize == 0) return;

    int *tmp = buf;
    for (size_t i = 0; i < bufsize; ++i) {
        printf("%d%c", tmp[i], ((i != bufsize - 1) ? (' ') : ('\n')));
        ++tmp;
    }
}
