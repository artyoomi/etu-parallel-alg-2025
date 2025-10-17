#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

typedef struct {
    unsigned size;
    int input;
    char* path;
    char** args;
} config_t;


// Print CLI usage instruction
void config_usage(char* prog_name);
// Initialize config structure with default values
void config_init(config_t* config);
void config_destroy(config_t* config);

// Parse cmd flag argument
void config_parse_cli_cmd(config_t* config, char* arg);
// Parse command line arguments
int config_parse_cli(config_t* config, int argc, char* argv[]);

#endif // CONFIG_H
