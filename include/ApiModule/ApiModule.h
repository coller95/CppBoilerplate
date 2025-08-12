#pragma once

// Compatibility header for ApiModule - provides static interface to ApiRouter
// This ensures that existing endpoint scripts continue to work
#include <ApiRouter/ApiRouter.h>
#include <functional>
#include <memory>
#include <vector>

namespace apimodule {
    // Import type aliases from compatibility header
    using IApiModule = apirouter::IApiModule;
    using IEndpointRegistrar = apirouter::IEndpointRegistrar;
    using HttpHandler = apirouter::HttpHandler;

    /**
     * ApiModule - Compatibility class that provides static access to ApiRouter
     * This class exists to maintain compatibility with existing endpoint scripts
     * while actually delegating all work to the ApiRouter singleton.
     */
    class ApiModule {
    public:
        /**
         * Register a module factory function (used by auto-registration)
         * @param factory Function that creates a new module instance
         */
        static void registerModuleFactory(std::function<std::unique_ptr<IApiModule>()> factory) {
            apirouter::ApiRouter::registerModuleFactoryGlobal(std::move(factory));
        }
        
        /**
         * Get the number of registered module factories
         * @return Number of registered module factories
         */
        static size_t getRegisteredModuleCount() {
            return apirouter::ApiRouter::getRegisteredModuleCountGlobal();
        }
        
        /**
         * Create instances of all registered modules
         * @return Vector of created module instances
         */
        static std::vector<std::unique_ptr<IApiModule>> createAllModules() {
            return apirouter::ApiRouter::createAllModulesGlobal();
        }
        
        /**
         * Initialize the API module system
         * @return True if initialization was successful
         */
        static bool initialize() {
            return apirouter::ApiRouter::initializeGlobal();
        }
        
        /**
         * Handle an HTTP request
         * @param path The request path
         * @param method The HTTP method
         * @param requestBody The request body content
         * @param responseBody Output parameter for response body
         * @param statusCode Output parameter for HTTP status code
         * @return True if the request was handled
         */
        static bool handleRequest(std::string_view path, std::string_view method, const std::string& requestBody, std::string& responseBody, int& statusCode) {
            return apirouter::ApiRouter::handleRequestGlobal(path, method, requestBody, responseBody, statusCode);
        }
        
        /**
         * Get the number of registered endpoints
         * @return Number of registered endpoints
         */
        static size_t getEndpointCount() {
            return apirouter::ApiRouter::getEndpointCountGlobal();
        }
        
        /**
         * Get a list of all registered endpoints
         * @return Vector of endpoint descriptions (path:method format)
         */
        static std::vector<std::string> getRegisteredEndpoints() {
            return apirouter::ApiRouter::getRegisteredEndpointsGlobal();
        }
    };
}
