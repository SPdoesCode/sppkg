#ifndef GLOBAL_H
#define GLOBAL_H

#include "parser.h"

#include <stddef.h>

typedef struct global {
    struct gpkg {
        char* name;
        char* version;
        char* file;
        char* src;
    }* packages;
    size_t gpkg_num;
} global_t;

global_t parser_global(char* src);
void add_global(char* filepath, package_t pkg, global_t* gbl);
void rm_global(package_t pkg, global_t* gbl);
char* write_global(global_t gbl);
void free_global(global_t* gbl);
int in_global(global_t gbl, package_t pkg);

#endif