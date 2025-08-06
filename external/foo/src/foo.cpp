#include "foo.h"
#include <iostream>

void external_print(const char* message) {
	std::cout << "External library says: " << message << std::endl;
}
