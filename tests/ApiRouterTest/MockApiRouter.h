#pragma once

#include <gmock/gmock.h>
#include <ApiRouter/IEndpointRegistrar.h>

namespace apirouter {

/**
 * Mock for ApiRouter testing - since ApiRouter is a singleton, we mock its dependencies instead
 * This mock can be used to simulate ApiRouter-like behavior in tests that need it
 */
class MockApiRouter {
public:
    MOCK_METHOD(bool, initialize, ());
    MOCK_METHOD(bool, handleRequest, 
               (std::string_view path, std::string_view method, const std::string& requestBody, 
                std::string& responseBody, int& statusCode));
    MOCK_METHOD(size_t, getEndpointCount, (), (const));
    MOCK_METHOD(std::vector<std::string>, getRegisteredEndpoints, (), (const));
    MOCK_METHOD(void, registerModuleFactory, 
               (std::function<std::unique_ptr<IApiModule>()> factory));
    MOCK_METHOD(size_t, getRegisteredModuleCount, (), (const));
    MOCK_METHOD(std::vector<std::unique_ptr<IApiModule>>, createAllModules, (), (const));
};

/**
 * Mock implementation of IEndpointRegistrar for testing
 */
class MockEndpointRegistrar : public IEndpointRegistrar {
public:
    MOCK_METHOD(void, registerHttpHandler, 
               (std::string_view path, std::string_view method, HttpHandler handler), 
               (override));
};

/**
 * Mock implementation of IApiModule for testing
 */
class MockApiModule : public IApiModule {
public:
    MOCK_METHOD(void, registerEndpoints, (IEndpointRegistrar& registrar), (override));
};

} // namespace apirouter
