// WebServerBindAddressTest.cpp
// Test that WebServer binds to the correct IP address when constructed
// (TDD: Failing test first)

#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <memory>
#include <string>

using namespace ::testing;

namespace webserver_test {

TEST(WebServerBindAddressTest, BindsToSpecifiedIpAddress)
{
    // Arrange: create WebServer with custom IP and port
    std::string ip = "0.0.0.0";
    int port = 12345;
    std::unique_ptr<WebServer> server = std::make_unique<WebServer>(ip, port);

    // Act: retrieve bind address (assume WebServer exposes a method for test)
    std::string actualIp = server->getBindIp();
    int actualPort = server->getBindPort();

    // Assert: should match what was passed in
    EXPECT_EQ(actualIp, ip);
    EXPECT_EQ(actualPort, port);
}

} // namespace webserver_test
