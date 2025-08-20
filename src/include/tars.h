#ifndef TARS_H
#define TARS_H

typedef struct outfiles {
    char** files;
    int file_num;
} outfiles_t;

void make_tar(char* folder, char* output);
void untar(char* tar, char* output);
outfiles_t read_tar(char* file);
void free_read(outfiles_t read_files);

#endif