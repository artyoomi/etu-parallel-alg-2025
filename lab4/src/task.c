#include "../../task.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void usage(char* name)
{
    printf("Usage: %s <K>\n", name);
}

void generate_double_array(double* array, size_t array_size, int seed) {
    if (array == NULL || array_size == 0) {
        return;
    }

    srand(seed);

    for (size_t i = 0; i < array_size; ++i) {
        array[i] = (double)rand() / RAND_MAX;
    }
}

// Fisher-Yates
void generate_binary_array(int* array, size_t array_size, int K, int seed)
{
    if (K > array_size || array_size == 0 || array == NULL) {
        array = NULL;
        return;
    }

    srand(seed);

    memset(array, 0, array_size * sizeof(int));

    int placed_count = 0;
    for (int i = 0; i < array_size && placed_count < K; ++i) {
        int remaining_positions = array_size - i;
        int remaining_ones = K - placed_count;

        if (rand() % remaining_positions < remaining_ones) {
            array[i] = 1;
            ++placed_count;
        }
    }
}

int task(int world_size, int world_rank, int n_args, char* args[])
{
    int retcode = 0;

    // Count of subordinate processes
    int workers_count = world_size - 1;

    int K = 0;
    if (world_rank == 0) {
        #ifndef MEASURE_TIME
        // Check command line arguments count
        if (n_args != 2) {
            retcode = -1;
            usage(args[0]);
        // Try to convert argument to positive number
        } else if (sscanf(args[1], "%d", &K) != 1 || K < 0) {
            retcode = errno;
            fprintf(stderr,
                    "Cannot convert input argument to positive interger\n");
        // Program needs to have K less or equal workers count to work properly
        } else if (K > workers_count) {
            retcode = -1;
            fprintf(stderr,
                    "Error: K value must be less or equal than workers count\n");
        }
        #else
        // Use this version to simplify time measurements
        // srand(time(NULL));
        // K = rand() % world_size;
        K = world_size - 1;
        #endif
    }

    #ifndef MEASURE_TIME
    // Send validation result to all processes
    MPI_Bcast(&retcode, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (retcode) {
        return retcode;
    }
    #endif

    MPI_Bcast(&K, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // If K is 0 then there is no processes for new communicator
    if (K == 0) {
        return retcode;
    }

    int* ones_placement = NULL;
    if (world_rank == 0) {
        ones_placement = (int*)malloc(world_size * sizeof(int));
        ones_placement[0] = 1;  // Root process must be in new communicator
        generate_binary_array(&ones_placement[1], workers_count, K, time(NULL));
    }

    int N = 0;
    MPI_Scatter(ones_placement, 1, MPI_INT, &N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (ones_placement) free(ones_placement);

    int color = (N == 1) ? (0) : (MPI_UNDEFINED);

    MPI_Comm local_comm = MPI_COMM_NULL;
    MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &local_comm);

    double *sendbuf = NULL;
    double recvbuf = 0.0F;
    if (world_rank == 0) {
        sendbuf = (double*)malloc(K * sizeof(double));
        if (!sendbuf) {
            retcode = errno;
            perror("Cannot allocate memory for sendbuf to send array\n");
            return retcode;
        }

        generate_double_array(sendbuf, K, time(NULL));
    }

    // Distribute double values between processes in local_comm
    if (local_comm != MPI_COMM_NULL) {
        int local_size = 0;
        MPI_Comm_size(local_comm, &local_size);
        int local_rank = 0;
        MPI_Comm_rank(local_comm, &local_rank);

        // Create prerequisites to ignore process with world rank 0 on Scatter
        int *send_counts = NULL;
        int *displs = NULL;
        if (world_rank == 0) {
            send_counts = (int*)malloc(local_size * sizeof(int));
            displs = (int*)malloc(local_size * sizeof(int));

            for (size_t i = 0; i < local_size; ++i) {
                send_counts[i] = (i == 0) ? (0) : (1);
                displs[i] = (i == 0) ? (0) : (i - 1);
            }
        }

        MPI_Scatterv(sendbuf, send_counts, displs, MPI_DOUBLE, &recvbuf, 1,
                     MPI_DOUBLE, 0, local_comm);

        #ifndef MEASURE_TIME
        if (world_rank != 0) {
            printf("Process with world_rank=%d (local_rank=%d) recieved value=%lf\n",
                   world_rank, local_rank, recvbuf);
        }
        #endif

        MPI_Comm_free(&local_comm);

        if (send_counts) free(send_counts);
        if (displs) free(displs);
    }

    if (sendbuf) free(sendbuf);

    return retcode;
}
