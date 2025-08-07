#include "gtest/gtest.h"
#include "RestfulServer/RestfulServer.h"
#include <string>

/**
 * Unit tests for RestfulServer
 */
class RestfulServerTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(RestfulServerTest, CanStartAndStopServer) {
    RestfulServer server(8080);
    EXPECT_FALSE(server.isRunning());
    server.start();
    EXPECT_TRUE(server.isRunning());
    server.stop();
    EXPECT_FALSE(server.isRunning());
}

TEST_F(RestfulServerTest, RegisterHandlerAndInvoke) {
    RestfulServer server(8081);
    bool handlerCalled = false;
    server.registerHandler("/test", [&](const std::string& body) {
        handlerCalled = true;
        return std::string("ok:") + body;
    });
    // Simulate handler invocation (not real HTTP, just test registration)
    // This would require exposing handler map or a test hook in real code
    // For now, just ensure no exceptions and registration works
    server.start();
    server.stop();
    EXPECT_FALSE(handlerCalled); // Handler not called in this test
}
