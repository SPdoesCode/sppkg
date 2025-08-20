#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>


typedef struct package {
    char* name;
    char* version;
    char* author;
    char* src;
    size_t dep_num;
    struct dep {
        char* dep;
        char* ver;
    } *deps;
    size_t build_num;
    struct build {
        char* step;
    } *builds;
} package_t;

typedef struct config {
    size_t repos_num;
    struct repo {
        char* name;
        char* link;
    } *repos;
} config_t;

package_t parse_package(char* src);
config_t parse_config(char* src);
void free_pkg(package_t* pkg);
void free_cfg(config_t* cfg);

#endif