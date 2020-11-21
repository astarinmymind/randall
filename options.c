#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "./options.h"

void read_options(
    int argc,
    char* argv[],
    struct opts* opts
) {
    opts->valid = false;
    opts->input = DEFAULT;
    opts->output = NONE;
    int c;
    while ((c = getopt(argc, argv, ":i:o:")) != -1) {
        switch(c) {
            case 'i':
                if (strcmp("rdrand", optarg) == 0) {
                    opts->input = RDRAND;
                } else if (strcmp("mrand48_r", optarg) == 0) {
                    opts->input = MRAND48_R;
                } else if ('/' == optarg[0]) {
                    opts->input = SLASH_F;
                    opts->r_src = optarg;
                } else {
                    break;
                }
                opts->valid = true;
                break;
            case 'o':
                if (strcmp("stdout", optarg) == 0) {
                    opts->output = STDOUT;
                } else {
                    opts->output = NUM;
                    opts->block_size = (atoi(optarg)) * 1024;
                }
                opts->valid = true;
                break;
            case ':':
                break;
            case '?':
                break;
        }
    }
    if (optind >= argc) {
        return;
    }
    opts->nbytes = atol(argv[optind]);
    if (opts->nbytes >= 0) {
        opts->valid = true;
        if (opts->output == NONE) {
            opts->block_size = opts->nbytes;
        }
    }
}
