#include <mpi.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void fill_array(int* array, size_t array_size, unsigned int seed)
{
    srand(seed);

    const int upper_bound = 1000;
    for (size_t i = 0; i < array_size; ++i) {
        array[i] = rand() % upper_bound;
    }
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    // Get rank
    int world_rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get amount of processes
    int world_size = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int root_rank = 0;

    // Create and fill array with random numbers
    const size_t array_size = world_size + 5;
    int* array = (int*)malloc(array_size * sizeof(int));
    if (!array) {
        perror("Can't allocate memory for array");
        goto free_memory;
    }
    fill_array(array, array_size, time(NULL) + world_rank);

    #ifdef DEBUG
    printf("Rank %d array: ", world_rank);
    for (size_t i = 0; i < array_size; ++i) {
        printf("%3d ", array[i]);
    }
    putchar('\n');
    #endif

    const size_t sendbuf_size = array_size * 2;
    int* sendbuf = (int*)malloc(sendbuf_size * sizeof(int));
    if (!sendbuf) {
        perror("Can't allocate memory for sendbuf");
        goto free_memory;
    }
    for (size_t i = 0; i < array_size; ++i) {
        sendbuf[i*2] = array[i];
        sendbuf[i*2 + 1] = world_rank;
    }

    // To get return value in form: result1, index1, result2, index2, ... 
    const size_t recvbuf_size = sendbuf_size;
    int* recvbuf = NULL;
    if (world_rank == root_rank) {
        recvbuf = (int*)malloc(recvbuf_size * sizeof(int));
        if (!recvbuf) {
            perror("Can't allocate memory for recvbuf");
            goto free_memory;
        }
    }

    MPI_Reduce(sendbuf, recvbuf, array_size, MPI_2INT, MPI_MAXLOC, root_rank, MPI_COMM_WORLD);

    if (world_rank == root_rank) {
        // Output all max values
        printf("%8s: ", "MAX");
        for (size_t i = 0; i < recvbuf_size; i+=2) {
            printf("%3d ", recvbuf[i]);
        }
        putchar('\n');

        // Output ranks of processes containing those max values
        printf("%8s: ", "RANK");
        for (size_t i = 1; i < recvbuf_size; i+=2) {
            printf("%3d ", recvbuf[i]);
        }
        putchar('\n');
    }

free_memory:
    free(array);
    free(sendbuf);
    free(recvbuf);

    return MPI_Finalize();
}
