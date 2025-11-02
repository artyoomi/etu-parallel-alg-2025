#include "../../task.h"

#include <mpi.h>
#include <stdio.h>

static inline void hello(int rank)
{
    printf("Hello from process %d\n", rank);
}

int task(int world_size, int world_rank, int n_args, char* args[])
{
    // Rank specific logic
    if (world_rank == 0) {
        #ifndef MEASURE_TIME
            hello(world_rank);
        #endif

        // Recieve rank of other processes
        int rank;
        // Loop is started from 1 just to abstractly say that we wait for
        // messages from processes with ranks from 1 to 7
        for (int i = 1; i < world_size; ++i) {
            MPI_Recv(&rank, 1, MPI_INT, i,
                     0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            #ifndef MEASURE_TIME
                hello(rank);
            #endif
        }
    } else {
        MPI_Send(&world_rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    return 0;
}
