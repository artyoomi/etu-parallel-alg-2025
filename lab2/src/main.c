#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "config.h"
#include "array_utils.h"

int main(int argc, char* argv[])
{
    for (size_t i = 0; i < argc; ++i) {
        printf("%s\n", argv[i]);
    }
    
    int retval = 0;

    // Get program config from command line arguments
    config_t config;
    config_init(&config);
    retval = config_parse_cli(&config, argc, argv);
    if (retval) {
        printf("Error: %s\n", strerror(retval));
        return -1;
    }

    size_t array_size = config.size;
    int* array = (int*)malloc(array_size * sizeof(int));
    if (!array) {
        fprintf(stderr, "Error: failed to allocate %ld bytes.\n", array_size);
        exit(EXIT_FAILURE);
    }

    // Checking the array formation method
    if (!config.input) {
        utils_generate_array(array, array_size);
    } else {
        for (size_t i = 0; i < array_size; ++i) {
            scanf("%d", &array[i]);
        }
    }

    // Get program execution result
    computation_result_t result = utils_max(array, array_size, config.path, config.args);

    // Check result
    if (!result.retval) {
        fprintf(stderr, "Error occured while computing array maximum\n");
    } else {
        printf("%d %lf", result.max_value, result.elapsed_time);
    }

    // Unitialize all data
    free(array);
    config_destroy(&config);
    
    return 0;
}
