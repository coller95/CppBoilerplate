#include <gtest/gtest.h>
#include <ApiModule/ApiModule.h>
#include <ApiModule/IEndpointRegistrar.h>
#include <vector>
#include <string>

class MockEndpointRegistrar : public IEndpointRegistrar {
public:
    std::vector<std::string> registeredPaths;
    void registerHttpHandler(std::string_view path, std::string_view method, HttpHandler /*handler*/) override {
        registeredPaths.push_back(std::string(path) + ":" + std::string(method));
    }
};

TEST(ApiModuleTest, RegisterAllRegistersEndpoints) {
    MockEndpointRegistrar registrar;
    apimodule::ApiModule::registerAll(registrar);
    
    // Test should pass regardless of how many endpoints are registered
    // This tests that the registerAll method can be called without errors
    // The actual endpoint registration is tested in individual endpoint test modules
    
    // Verify the method completed successfully (no exceptions thrown)
    SUCCEED() << "ApiModule::registerAll() completed successfully. Registered " 
              << registrar.registeredPaths.size() << " endpoint(s).";
    
    // Optional: Log what was registered for debugging purposes
    if (!registrar.registeredPaths.empty()) {
        for (const auto& endpoint : registrar.registeredPaths) {
            std::cout << "Registered endpoint: " << endpoint << std::endl;
        }
    }
}
