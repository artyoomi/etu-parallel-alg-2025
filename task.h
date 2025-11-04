#ifndef TASK_H
#define TASK_H

#include <mpi.h>
#include <errno.h>

// Variadic arguments behavior defined by task itself
int task(int world_size, int world_rank, int n_args, char* args[]);

#endif
