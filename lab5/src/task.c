#include "../../task.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int task(int world_size, int world_rank, int n_args, char* args[])
{
    int retcode = 0;

    if (world_size % 2 == 1) {
        retcode = 1;
        if (world_rank == 0) {
            fprintf(stderr, "Process count must be even!\n");
        }
        return retcode;
    }

    int N = world_size / 2;

    // Create cartesian grid
    MPI_Comm grid_comm;
    int dims[2] = {2, N};
    int periods[2] = {0, 0};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &grid_comm);

    // Create first subgrid
    MPI_Comm row1_comm;
    int remain_dims1[] = {1, 0};
    MPI_Cart_sub(grid_comm, remain_dims1, &row1_comm);

    // Create second subgrid
    MPI_Comm row2_comm;
    int remain_dims2[] = {0, 1};
    MPI_Cart_sub(grid_comm, remain_dims2, &row1_comm);

    // Generate A value
    float A = 0;
    if (world_rank == 0 || world_rank == N) {
        srand(time(NULL) + world_rank);
        A = (float)(rand() % 10000);
    }

    if (row1_comm != MPI_COMM_NULL) {
        MPI_Bcast(&A, 1, MPI_FLOAT, 0, row1_comm);
    } else {
        MPI_Bcast(&A, 1, MPI_FLOAT, N, row2_comm);
    }

    #ifndef MEASURE_TIME
    printf("rank=%d: recieved A=%f\n", world_rank, A);
    #endif

    return retcode;
}
