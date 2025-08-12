#pragma once

#include <gmock/gmock.h>
#include <ApiRouter/IApiRouter.h>

namespace apirouter {

/**
 * Mock implementation of IApiRouter for testing
 */
class MockApiRouter : public IApiRouter {
public:
    MOCK_METHOD(bool, initialize, (), (override));
    MOCK_METHOD(bool, handleRequest, 
               (std::string_view path, std::string_view method, const std::string& requestBody, 
                std::string& responseBody, int& statusCode), 
               (override));
    MOCK_METHOD(size_t, getEndpointCount, (), (const, override));
    MOCK_METHOD(std::vector<std::string>, getRegisteredEndpoints, (), (const, override));
    MOCK_METHOD(void, registerModuleFactory, 
               (std::function<std::unique_ptr<IApiModule>()> factory), 
               (override));
    MOCK_METHOD(size_t, getRegisteredModuleCount, (), (const, override));
    MOCK_METHOD(std::vector<std::unique_ptr<IApiModule>>, createAllModules, (), (const, override));
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
