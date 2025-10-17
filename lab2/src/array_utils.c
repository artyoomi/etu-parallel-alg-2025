#include "array_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX(i, j) (((i) > (j)) ? (i) : (j))

int utils_generate_array(int* array, size_t array_size)
{
    if (array == NULL)
        return EINVAL;

    srand(time(NULL));

    const int upper_bound = 100;
    for (size_t i = 0; i < array_size; ++i)
        array[i] = rand() % 100;

    return 0;
}

void utils_print_array(const int* array, size_t array_size)
{
    for (size_t i = 0; i < array_size; ++i)
        printf("%d ", array[i]);
    putchar('\n');   
}


// Implementation may be a lot better, but now it works so I will use it
char* utils_array_to_string(const int* array, size_t array_size)
{
    if (array_size == 0)
        return strdup("");
    
    // Estimate maximum length needed
    // Each int could be up to 11 chars (-2147483648) + space
    size_t max_length = array_size * 12 + 1;
    char* result = (char*)malloc(max_length * sizeof(char));
    if (result == NULL) {
        return NULL;
    }
    
    // Build the string
    char* current = result;
    for (size_t i = 0; i < array_size; i++) {
        if (i > 0) {
            *current++ = ' ';
        }
        current += sprintf(current, "%d", array[i]);
    }
    *current = '\0';
    
    return result;
}

/* Program, specified in cmd must get array size and array on input and
   return max of array and elapsed time separated with space in stdout. */
computation_result_t utils_max(int* array, size_t array_size,
                               const char* path, char* const args[])
{
    computation_result_t result;
    // First fd for read, second fd is for write
    int pipefd[2];
   
    pid_t pid = fork();
    if (pid == -1) {
        result.retval = EAGAIN;
    } else if (pid == 0) {
        char* input = utils_array_to_string(array, array_size);
        // Write data to child stdin
        ssize_t bytes_write = write(pipefd[0], array, sizeof(input));
        
        // Close write end of pipe
        close(pipefd[0]);

        // Link stdout with pipe read end
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        execv(path, args);

        // If this part of code was reached, then execv failed
        perror("execv failed"); 
    } else {
        // Close read end of pipe
        close(pipefd[1]);        

        // Capture output of child process
        char output[1024];
        ssize_t bytes_read = read(pipefd[0], output, sizeof(output) - 1);
        close(pipefd[0]);

        if (bytes_read > 0) {
            output[bytes_read] = '\0';
            
            // Parse output: "max_number elapsed_time"
            if (sscanf(output, "%d %lf", &result.max_value, &result.elapsed_time) == 2) {
                result.retval = 0;
            } else {
                fprintf(stderr, "Failed to parse MPI output: %s\n", output);
                result.retval = -1;
            }
        } else {
            fprintf(stderr, "No output from program");
            result.retval = -1;
        }
    }

    int status;
    waitpid(pid, &status, 0);

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "MPI program exited abnormally\n");
        result.retval = WEXITSTATUS(status);
    }

    return result;
}
