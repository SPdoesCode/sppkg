#include "include/parser.h"

#include <stdio.h>
#include <string.h>

int main() {
    char* source = "name = \"test\"\0";
    package_t pkg = parse_package(source);

    if (pkg.name){ if (strcmp(pkg.name, "test")==0) printf("TEST PASSED!"); }
    else printf("TEST FAILED!");
}