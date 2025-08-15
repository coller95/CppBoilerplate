#include <gtest/gtest.h>
#include <EndpointHello/EndpointHello.h>
#include <ApiRouter/ApiRouter.h>
#include <vector>
#include <string>
#include <memory>

class MockEndpointRegistrar : public apirouter::IEndpointRegistrar {
public:
    std::vector<std::string> registeredPaths;
    void registerHttpHandler(std::string_view path, std::string_view method, apirouter::HttpHandler /*handler*/) override {
        registeredPaths.push_back(std::string(path) + ":" + std::string(method));
    }
};

TEST(EndpointHelloAutoRegistrationTest, EndpointHelloIsAutoRegisteredWithApiRouter) {
    // Check that at least one module factory is registered
    size_t moduleCount = apirouter::ApiRouter::getRegisteredModuleCountGlobal();
    ASSERT_GE(moduleCount, 1U) << "No endpoint modules were auto-registered";
    
    // Create instances of all registered modules
    auto modules = apirouter::ApiRouter::createAllModulesGlobal();
    ASSERT_EQ(modules.size(), moduleCount) << "Module creation count mismatch";
    
    // Check if any of the modules is EndpointHello
    bool foundEndpointHello = false;
    for (const auto& module : modules) {
        ASSERT_NE(module, nullptr) << "Module instance is null";
        
        // Try to dynamic_cast to EndpointHello to verify the type
        auto helloEndpoint = dynamic_cast<endpointhello::EndpointHello*>(module.get());
        if (helloEndpoint != nullptr) {
            foundEndpointHello = true;
            
            // Verify the module can register endpoints
            MockEndpointRegistrar registrar;
            helloEndpoint->registerEndpoints(registrar);
            
            // Verify that the endpoint registered its handler correctly
            ASSERT_EQ(registrar.registeredPaths.size(), 1U) << "EndpointHello should register exactly one endpoint";
            EXPECT_EQ(registrar.registeredPaths[0], "/hello:GET") << "EndpointHello should register /hello:GET endpoint";
            
            break;
        }
    }
    
    ASSERT_TRUE(foundEndpointHello) << "EndpointHello was not found in auto-registered modules";
}

TEST(EndpointHelloAutoRegistrationTest, ApiRouterCanInstantiateAllRegisteredModules) {
    // Get count of registered module factories
    size_t moduleCount = apirouter::ApiRouter::getRegisteredModuleCountGlobal();

    // Create instances of all registered modules
    auto modules = apirouter::ApiRouter::createAllModulesGlobal();
    ASSERT_EQ(modules.size(), moduleCount) << "Not all modules could be instantiated";
    
    // Verify each module can be used to register endpoints
    for (const auto& module : modules) {
        ASSERT_NE(module, nullptr) << "Module instance is null";
        
        MockEndpointRegistrar registrar;
        module->registerEndpoints(registrar);
        
        // The test passes if no exceptions are thrown
        SUCCEED();
    }
}
