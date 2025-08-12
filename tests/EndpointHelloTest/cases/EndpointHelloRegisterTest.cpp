#include <gtest/gtest.h>
#include <EndpointHello/EndpointHello.h>
#include <ApiRouter/IApiRouter.h>
#include <vector>
#include <string>

class MockEndpointRegistrar : public apirouter::IEndpointRegistrar {
public:
    std::vector<std::string> registeredPaths;
    void registerHttpHandler(std::string_view path, std::string_view method, apirouter::HttpHandler /*handler*/) override {
        registeredPaths.push_back(std::string(path) + ":" + std::string(method));
    }
};

TEST(EndpointHelloTest, RegisterEndpointsCanBeRegistered) {
    MockEndpointRegistrar registrar;
    endpointhello::EndpointHello endpoint;
    endpoint.registerEndpoints(registrar);
    
    // Verify that the endpoint registered its handler
    ASSERT_EQ(registrar.registeredPaths.size(), 1U);
    EXPECT_EQ(registrar.registeredPaths[0], "/hello:GET");
}
