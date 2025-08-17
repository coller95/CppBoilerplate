#include <gtest/gtest.h>
#include <EndpointHello/EndpointHello.h>
#include <ApiRouter/IEndpointRegistrar.h>
#include <vector>
#include <string>

class MockEndpointRegistrar : public apirouter::IEndpointRegistrar {
public:
	std::vector<std::string> registeredPaths;
	void registerHttpHandler(std::string_view path, std::string_view method, apirouter::HttpHandler /*handler*/) override {
		registeredPaths.push_back(std::string(path) + ":" + std::string(method));
	}
};

TEST(EndpointHelloTest, MetaprogrammingRegistrationWorks) {
	MockEndpointRegistrar registrar;
	endpointhello::EndpointHello endpoint;
	
	// Test the metaprogramming auto-registration method
	endpoint.registerAvailableMethods(registrar, "/hello");
	
	// Verify that the endpoint registered its handler via metaprogramming
	ASSERT_EQ(registrar.registeredPaths.size(), 1U);
	EXPECT_EQ(registrar.registeredPaths[0], "/hello:GET");
}

TEST(EndpointHelloTest, BaseRegistrationMethodWorks) {
	MockEndpointRegistrar registrar;
	endpointhello::EndpointHello endpoint;
	
	// Test the base IApiModule registerEndpoints method (calls metaprogramming internally)
	endpoint.registerEndpoints(registrar);
	
	// Should work the same way via the base class interface
	ASSERT_EQ(registrar.registeredPaths.size(), 1U);
	EXPECT_EQ(registrar.registeredPaths[0], "/hello:GET");
}
