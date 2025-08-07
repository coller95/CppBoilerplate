#include "PrintHello/PrintHello.h"
#include <iostream>

namespace print_hello {

/**
 * Prints a hello message to standard output.
 */
void PrintHello::print() const {
    std::cout << "Hello from printHello!" << std::endl;
}

} // namespace print_hello
