#include "foo_c.h"
#include <stdio.h>

void external_print_c(const char* message) {
    printf("C library says: %s\n", message);
}
