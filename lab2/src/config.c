#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>


void config_usage(char* prog_name)
{
    printf("Usage: %s [OPTIONS]", prog_name);
    puts("\t-h,--help - to show this message");
    puts("\t-s,--size <NUM> - size of array to generate (default is 50)");
    puts("\t-i,--input - to get array from input instead of generating");
}

void config_init(config_t* config)
{
    config->size = 50;
    config->input = 0;
    config->path = NULL;
    config->args = NULL;
}

void config_destroy(config_t* config)
{
    config->size = 0;
    config->input = 0;
    free(config->path);
    free(config->args);
}

void config_parse_cli_cmd(config_t* config, char* arg)
{
    char *saveptr;
    char *token = strtok_r(arg, " ", &saveptr);    
    if (token != NULL) {
        // Save path to main executable
        config->path = strdup(token);
        
        size_t scaler = 2;
        size_t capacity = 5;
        size_t args_count = 0;
        
        // Allocate memory for args (+1 added to place NULL after all)
        config->args = (char**)malloc((capacity + 1) * sizeof(char*));
        
        // Save args to allocated array
        while ((token = strtok_r(NULL, " ", &saveptr)) != NULL) {
            // +1 added to make space for NULL terminator
            if (args_count + 1 >= capacity) {
                capacity += scaler;
                config->args = (char**)realloc(config->args, (capacity + 1) * sizeof(char*));
            }
            (config->args)[args_count++] = strdup(token);
        }
        // Add NULL to use mark end
        (config->args)[args_count] = NULL;
    }
}

int config_parse_cli(config_t* config, int argc, char* argv[])
{
    if (config == NULL) {
        return EINVAL;
    }
    
    char* opts = "hs:vi:c:";
    struct option longopts[] = {
        {"help", no_argument, NULL, 'h'},
        {"size", required_argument, NULL, 's'},
        {"verbose", no_argument, NULL, 'v'},
        {"input", no_argument, NULL, 'i'},
        {"cmd", required_argument, NULL, 'c'},
        {NULL, no_argument, NULL, 0}
    };
    
    int opt = 0;
    while ((opt = getopt_long(argc, argv, opts, longopts, &optind)) != -1) {
        switch (opt) {
        case 'h':
            config_usage(argv[0]);
            return 0;
        case 's':
            // Maybe not so good as regex but for this task that's enough
            config->size = atoi(optarg);
            if (config->size <= 1)
                return EINVAL;
            break;
        case 'i':
            config->input = 1;
            break;
        case 'c':
            config_parse_cli_cmd(config, optarg);
            break;
        case '?':
            return EOPNOTSUPP;
        }
    }

    return 0;
}
