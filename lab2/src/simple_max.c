#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define MAX(i, j) (((i) > (j)) ? (i) : (j))

int max(int* array, size_t array_size, int* max_buf, double* time)
{
    if (array_size == 0)
        return EINVAL;
    
    int max = array[0];

    clock_t start = clock();
    for (size_t i = 0; i < array_size; ++i) {
        max = MAX(max, array[i]);
    }
    clock_t end = clock();

    *max_buf = max;
    *time = (double)(end - start) / CLOCKS_PER_SEC;

    return 0;
}

int main()
{
    size_t array_size;
    scanf("%lu", &array_size);

    int* array = (int*)malloc(array_size * sizeof(int));
    for (size_t i = 0; i < array_size; ++i) {
        scanf("%d", &array[i]);
    }

    int max_val = 0;
    double time_elapsed = 0;

    max(array, array_size, &max_val, &time_elapsed);
    printf("%d %lf\n", max_val, time_elapsed);

    free(array);
}
