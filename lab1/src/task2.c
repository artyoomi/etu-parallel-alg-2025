#include <mpi.h>
#include <stdio.h>

static inline void hello(int rank)
{
    printf("Hello from process %d\n", rank);
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Rank specific logic
    if (world_rank == 0) {
        #ifdef MEASURE_TIME
            double start = MPI_Wtime();
        #endif
        // Get the number of processes
        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        #ifdef VERBOSE
            hello(world_rank);
        #endif

        // Recieve rank of other processes
        int rank;
        // Loop is started from 1 just to abstractly say that we wait for
        // messages from processes with ranks from 1 to 7
        for (int i = 1; i < world_size; ++i) {
            MPI_Recv(&rank, 1, MPI_INT, i,
                     0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            #ifdef VERBOSE
                hello(rank);
            #endif
        }
        #ifdef MEASURE_TIME
            printf("%lf\n", MPI_Wtime() - start);
        #endif
    } else {
        MPI_Send(&world_rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    
    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}
