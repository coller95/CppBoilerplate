
#include <gtest/gtest.h>
#include <PrintHello/PrintHello.h>
#include <sstream>
#include <iostream>

/**
 * Unit tests for print_hello::PrintHello class
 */

TEST(PrintHelloTest, PrintsHelloWorld) {
    // Redirect std::cout to a stringstream
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

    // Call the method
    print_hello::PrintHello printer;
    printer.print();

    // Restore std::cout
    std::cout.rdbuf(oldCout);

    // Check output
    std::string output = buffer.str();
    EXPECT_NE(output.find("Hello from printHello!"), std::string::npos);
}
