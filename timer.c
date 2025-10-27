#include <mpi.h>
#include <stdio.h>

#include "task.h"

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

    #ifdef MEASURE_TIME
    MPI_Barrier();
    double start_time, end_time;
    if (world_rank == 0) {
        start_time = MPI_Wtime();
    }
    #endif

    if (task(world_size, world_rank, root_rank)) {
        perror("Unable to recover from error in main task");
        goto finalize;
    }
    
    #ifdef MEASURE_TIME
    if (world_rank == 0) {
        end_time = MPI_Wtime();
        printf("%lf\n", end_time - start_time);    
    }
    #endif

finalize:
    return MPI_Finalize();
}
