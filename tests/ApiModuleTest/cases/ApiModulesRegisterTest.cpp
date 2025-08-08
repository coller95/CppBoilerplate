#include <gtest/gtest.h>
#include <ApiModule/ApiModules.h>
#include <ApiModule/IEndpointRegistrar.h>
#include <vector>
#include <string>

class MockEndpointRegistrar : public IEndpointRegistrar {
public:
    std::vector<std::string> registeredPaths;
    void registerHttpHandler(std::string_view path, std::string_view method, HttpHandler handler) override {
        registeredPaths.push_back(std::string(path) + ":" + std::string(method));
    }
};

TEST(ApiModulesTest, RegisterAllRegistersEndpoints) {
    MockEndpointRegistrar registrar;
    apimodule::ApiModules::registerAll(registrar);
    // This will fail until at least one ApiModule registers an endpoint
    EXPECT_FALSE(registrar.registeredPaths.empty()) << "No endpoints registered by ApiModules!";
}
