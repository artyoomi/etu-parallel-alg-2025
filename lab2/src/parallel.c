#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "array_utils.h"

int main(int argc, char* argv[])
{
    size_t array_size;
    
    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank != 0) {
        // Get array size from main process
        unsigned array_size;
        MPI_Recv(&array_size, 1, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (array_size != 0) {
            // Get array from main process
            int* array = (int*)malloc(array_size * sizeof(int));
            MPI_Recv(array, array_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Find max
            int max = array[0];
            for (size_t i = 1; i < array_size; ++i)
                max = MAX(max, array[i]);

            // Send max to main process
            MPI_Send(&max, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

            free(array);
        }
    } else {
        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        if (world_size == 1) {
            fprintf(
                stderr,
                "Error: main process has no functionality to find max without workers, please specify -n greater than 1"
            );
            return -1;
        }
    
        // if (config.verbose) {
            printf("Rank_0 array: ");
            // print_array(array, config.size);
        // }
    
        double estimation = (double)config.size / (world_size - 1);
        /* It is not make sense no send less than 2 numbers to other proocesses.
           But it is good question what will be better: to send this to numbers
           to other process or calculate it by yourself... */
        const unsigned min_num_per_process = 2;
        unsigned num_per_process = min_num_per_process;
        if (estimation > (double)min_num_per_process)
            num_per_process = (unsigned)estimation;

        int* array_ptr = array;
        int max = array[0];

        unsigned overhead_num_per_process = num_per_process +
            ((config.size >= (world_size - 1) * 2) ? (config.size - num_per_process * (world_size - 1)) : (0));

        if (config.verbose) {
            printf("Rank_1 array: ");
            print_array(array_ptr, overhead_num_per_process);
        }

        MPI_Send(&overhead_num_per_process, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(
            array_ptr,
            overhead_num_per_process,
            MPI_INT,
            1, 0, MPI_COMM_WORLD
        );
        array_ptr = array_ptr + overhead_num_per_process;

        /* i = 2 to ignore main process and process 1 that already recieved
           his part of array. */
        for (size_t i = 2; i < world_size; ++i) {
            if (config.verbose) {
                printf("Rank_%ld array: ", i);
                print_array(array_ptr, num_per_process);
            }

            MPI_Send(&num_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(
                array_ptr, num_per_process,
                MPI_INT, i, 0, MPI_COMM_WORLD
            );
            array_ptr = array_ptr + num_per_process;
        }

        int buf;
        for (size_t i = 1; i < world_size; ++i) {
            MPI_Recv(&buf, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            max = MAX(max, buf);
        }

        printf("%d\n", max);

        free(array);
    }
}
