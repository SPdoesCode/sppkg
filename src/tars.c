#include "include/tars.h"

#include <libtar.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

void make_tar(char *folder, char *output) {

}
void untar(char *tar, char *output) {

}

outfiles_t read_tar(char* file) {
    TAR *tar;
    if (tar_open(&tar, file, NULL, O_RDONLY, 0, 0) != 0) {
        printf("ERROR: Couldnt read tar %s", file);
        exit(0);
    }
    size_t count = 0;
    size_t capacity = 10;
    char **files = malloc(capacity * sizeof(char*));
    while (th_read(tar) == 0) {
        if (count >= capacity) {
            capacity *= 2;
            char **tmp = realloc(files, capacity * sizeof(char*));
            if (!tmp) {
                perror("realloc failed");
                break;
            }
            files = tmp;
        }
        char *filename = strdup(th_get_pathname(tar));
        files[count++] = filename;

        tar_skip_regfile(tar);
    }

    tar_close(tar);

    return (outfiles_t) {files, count};
}

void free_read(outfiles_t files) {
    for (int i = 0; i < files.file_num; i++) {
        free(files.files[i]);
    }
    free(files.files);
}