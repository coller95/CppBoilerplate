#include <iostream>
#include "printHello.h"
#include "foo.h"
#include "foo_c.h"

int main() {
    printHello();
    external_print("Hello from the external C++ library!");
    external_print_c("Hello from the external C library!");
    return 0;
}