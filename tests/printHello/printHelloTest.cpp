#include <gtest/gtest.h>
#include <printHello.h>
#include <sstream>
#include <iostream>

/**
 * Unit tests for printHello function
 */

TEST(PrintHelloTest, PrintsHelloWorld) {
    // Redirect std::cout to a stringstream
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

    // Call the function
    printHello();

    // Restore std::cout
    std::cout.rdbuf(oldCout);

    // Check output
    std::string output = buffer.str();
    EXPECT_NE(output.find("Hello from printHello!"), std::string::npos);
}
