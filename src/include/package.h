#ifndef PACKAGE_H
#define PACKAGE_H

#include "parser.h"

int package_install(package_t pkg);
int package_uninstall(package pkg);
int package_lookup(char* name);

#endif