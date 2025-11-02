#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include "../../task.h"
#include "../../common.h"

#define MAX(i, j) (((i) > (j)) ? (i) : (j))

#ifdef MEASURE_TIME
#undef DEBUG
#endif

void usage(const char* exe)
{
    printf("Usage: %s <array_size>\n", exe);
}

// This task doesn't use root_rank value
int task(int world_size, int world_rank, int argc, char* argv[])
{
    int retval = 0;

    if (world_rank == 0) {
        int current_worker = 1;

        int workers_count = world_size - 1;
        int real_workers_count = workers_count;

        if (workers_count == 0) {
            retval = errno;
            perror("There are too few workers");
            goto finalize_redudant;
        }

        // Read array size from args list
        size_t array_size = 0;
        if (argc == 2) {
            if (argv[1] != NULL && sscanf(argv[1], "%lu", &array_size) == 1) {
                if (array_size < 0) {
                    retval = EINVAL;
                    perror("Array size is negative");
                    goto finalize_redudant;
                }
            } else {
                retval = EINVAL;
                perror("Invalid string argument (must contain array size)");
                goto finalize_redudant;
            }
        } else {
            retval = EINVAL;
            usage(argv[0]);
            goto finalize_redudant;
        }

        if (array_size == 0) {
            goto finalize_redudant;
        }

        #ifdef DEBUG
        printf("Recieved array size: %u\n", array_size);
        #endif

        // Generate array
        int* array = (int*)malloc(array_size * sizeof(int));
        if (!array) {
            retval = errno;
            perror("Can't allocate memory for array");
            goto finalize_redudant;
        }
        generate_array(array, array_size, time(NULL) + world_rank);

        #ifdef DEBUG
        printf("Array elements: ");
        for (size_t i = 0; i < array_size; ++i) {
            printf("%d ", array[i]);
        }
        putchar('\n');
        #endif

        double estimation = (double)array_size / workers_count;
        #ifdef DEBUG
        printf("Estimation: %lf\n", estimation);
        #endif

        /* It is not make sense no send less than 2 numbers to other processes.
           But it is good question what will be better: to send this two numbers
           to other process or calculate it by yourself... */
        // Calculate number of array elements for each process
        const size_t min_num_per_process = 1;
        size_t num_per_process = min_num_per_process;
        if ((unsigned)estimation > min_num_per_process) {
            num_per_process = (unsigned)estimation;
        }

        #ifdef DEBUG
        printf("Array elements per process: %u\n", num_per_process);
        #endif

        int* array_ptr = array;
        int max = array[0];

        // Array size is less than workers count => decrease workers count
        if (array_size < workers_count) {
            workers_count = array_size / 2 + 1;
            #ifdef DEBUG
            printf("Workers count decreased to %d\n", workers_count);
            #endif
        }

        size_t overhead_num_per_process = num_per_process + array_size % workers_count;

        #ifdef DEBUG
        printf("Array elements per process with overhead: %d\n", overhead_num_per_process);
        #endif

        // Process with rank 1 will sort array part with overhead
        MPI_Send(&overhead_num_per_process, 1, MPI_UNSIGNED_LONG_LONG,
                 current_worker, 0, MPI_COMM_WORLD);
        MPI_Send(array_ptr, overhead_num_per_process, MPI_INT,
                 current_worker, 0, MPI_COMM_WORLD);

        #ifdef DEBUG
        printf("%d array elements were sent to worker %d: ", overhead_num_per_process, current_worker);
        for (int* tmp = array_ptr; tmp < array_ptr + overhead_num_per_process; ++tmp) {
            printf("%d ", *tmp);
        }
        putchar('\n');
        #endif

        array_ptr = array_ptr + overhead_num_per_process;
        ++current_worker;

        for (; current_worker < workers_count + 1; ++current_worker) {
            MPI_Send(&num_per_process, 1, MPI_UNSIGNED_LONG_LONG,
                     current_worker, 0, MPI_COMM_WORLD);
            MPI_Send(array_ptr, num_per_process, MPI_INT,
                     current_worker, 0, MPI_COMM_WORLD);

            #ifdef DEBUG
            printf("%d array elements were sent to worker %d: ", num_per_process, current_worker);
            for (int* tmp = array_ptr; tmp < array_ptr + num_per_process; ++tmp) {
                printf("%d ", *tmp);
            }
            putchar('\n');
            #endif

            array_ptr = array_ptr + num_per_process;
        }

        // Merge computation results
        int recieved_max = 0;
        for (size_t i = 1; i < workers_count + 1; ++i) {
            MPI_Recv(&recieved_max, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            max = MAX(max, recieved_max);

            #ifdef DEBUG
            printf("Max value equal to %d was received from process with rank %ld\n", recieved_max, i);
            #endif
        }
        free(array);

        #ifndef MEASURE_TIME
        printf("%d\n", max);
        #endif


    // Send zero size for redudant workers
    finalize_redudant:
        // Send size 0 to redudant workers
        size_t zero_buf = 0;
        for (int i = current_worker; i < real_workers_count + 1; ++i) {
            MPI_Send(&zero_buf, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);

            #ifdef DEBUG
            printf("Size equal to 0 was sent to process with rank %d\n", i);
            #endif
        }
    } else {
        // Get array size from main process
        size_t subarray_size = 0;
        MPI_Recv(&subarray_size, 1, MPI_UNSIGNED_LONG_LONG, 0, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        #ifdef DEBUG
        fprintf(stderr, "Recieved in %d subarray size is %ld\n", world_rank, subarray_size);
        #endif
        if (subarray_size != 0) {
            // Get array from main process
            int* subarray = (int*)malloc(subarray_size * sizeof(int));
            if (!subarray) {
                retval = errno;
                perror("Can't allocate memory for subarray");
                MPI_Abort(MPI_COMM_WORLD, retval);
            }
            MPI_Recv(subarray, subarray_size, MPI_INT, 0, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);

            // Find max of subarray
            int max = subarray[0];
            for (size_t i = 1; i < subarray_size; ++i) {
                max = MAX(max, subarray[i]);
            }

            // Send subarray max to main process
            MPI_Send(&max, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

            free(subarray);
        }
    }

    return retval;
}
