#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <WebServerBackendMongoose/WebServerBackendMongoose.h>
#include "../MockWebServerBackendMongoose.h"

using ::testing::_;
using ::testing::Return;

namespace {

class WebServerBackendMongooseInterfaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Mock will be created in individual tests when needed
    }
};

} // anonymous namespace

TEST_F(WebServerBackendMongooseInterfaceTest, ImplementsIWebServerInterface) {
    // Verify that WebServerBackendMongoose implements IWebServer directly
    webserverbackendmongoose::WebServerBackendMongoose module("127.0.0.1", 8080);
    webserver::IWebServer* interface = &module;
    EXPECT_NE(interface, nullptr);
}

TEST_F(WebServerBackendMongooseInterfaceTest, CanBeCastToBaseInterface) {
    // Verify interface hierarchy works correctly
    webserverbackendmongoose::WebServerBackendMongoose module("192.168.1.1", 9090);
    
    // Should be usable as IWebServer
    webserver::IWebServer* backend = &module;
    EXPECT_EQ(backend->getBindAddress(), "192.168.1.1");
    EXPECT_EQ(backend->getPort(), 9090);
    EXPECT_FALSE(backend->isRunning());
}

TEST_F(WebServerBackendMongooseInterfaceTest, ProvidesMongooseSpecificMethods) {
    // Verify Mongoose-specific interface methods are available
    webserverbackendmongoose::WebServerBackendMongoose module("localhost", 3000);
    
    // Should be able to call Mongoose-specific configuration methods
    EXPECT_NO_THROW(module.setDocumentRoot("/var/www"));
    EXPECT_NO_THROW(module.setMaxRequestSize(2048));
    EXPECT_NO_THROW(module.setConnectionTimeout(60));
    EXPECT_FALSE(module.enableSSL("/invalid/cert", "/invalid/key")); // Should return false for invalid files
}

// TODO: Add mock tests when needed for testing dependent components
