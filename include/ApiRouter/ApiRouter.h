#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <functional>
#include "ApiRouter/IEndpointRegistrar.h"

namespace apirouter {

/**
 * ApiRouter - Singleton Router for managing HTTP endpoints and auto-registration
 * 
 * This router implements the singleton pattern to provide a single,
 * globally accessible instance for endpoint registration and request routing.
 * It automatically discovers and registers endpoint modules that follow
 * the auto-registration pattern.
 * 
 * Thread-safe singleton implementation using Meyer's singleton pattern.
 */
class ApiRouter : public IEndpointRegistrar {
public:
    /**
     * Get the singleton instance of the API router
     * Thread-safe implementation using Meyer's singleton pattern
     * @return Reference to the singleton instance
     */
    static ApiRouter& getInstance();

    /**
     * Register a module factory globally using the singleton router
     * @param factory Factory function that creates the module instance
     */
    static void registerModuleFactoryGlobal(std::function<std::unique_ptr<IApiModule>()> factory);
    
    /**
     * Get the number of registered module factories globally
     * @return Number of registered module factories
     */
    static size_t getRegisteredModuleCountGlobal();
    
    /**
     * Create instances of all registered modules globally
     * @return Vector of created module instances
     */
    static std::vector<std::unique_ptr<IApiModule>> createAllModulesGlobal();

    /**
     * Handle an HTTP request globally using the singleton router
     * @param path The request path
     * @param method The HTTP method
     * @param requestBody The request body content
     * @param responseBody Output parameter for response body
     * @param statusCode Output parameter for HTTP status code
     * @return True if the request was handled
     */
    static bool handleRequestGlobal(std::string_view path, std::string_view method, const std::string& requestBody, std::string& responseBody, int& statusCode);

    /**
     * Initialize the router globally using the singleton instance
     * @return True if initialization was successful
     */
    static bool initializeGlobal();

    /**
     * Get the number of registered endpoints globally
     * @return Number of registered endpoints
     */
    static size_t getEndpointCountGlobal();

    /**
     * Get a list of all registered endpoints globally
     * @return Vector of endpoint descriptions (path:method format)
     */
    static std::vector<std::string> getRegisteredEndpointsGlobal();

    /**
     * Destructor - ensures proper cleanup
     */
    ~ApiRouter();

    // Core ApiRouter functionality
    bool initialize();
    bool handleRequest(std::string_view path, std::string_view method, const std::string& requestBody, std::string& responseBody, int& statusCode);
    size_t getEndpointCount() const;
    std::vector<std::string> getRegisteredEndpoints() const;
    void registerModuleFactory(std::function<std::unique_ptr<IApiModule>()> factory);
    size_t getRegisteredModuleCount() const;
    std::vector<std::unique_ptr<IApiModule>> createAllModules() const;

    // IEndpointRegistrar interface implementation
    void registerHttpHandler(std::string_view path, std::string_view method, HttpHandler handler) override;

    // Delete copy constructor and assignment operator (singleton pattern)
    ApiRouter(const ApiRouter&) = delete;
    ApiRouter& operator=(const ApiRouter&) = delete;
    
    // Delete move constructor and assignment (singleton should not be moved)
    ApiRouter(ApiRouter&&) = delete;
    ApiRouter& operator=(ApiRouter&&) = delete;

private:
    /**
     * Private constructor for singleton pattern
     */
    ApiRouter();

    // Thread-safe access to endpoints and modules
    mutable std::mutex _mutex;
    
    // Endpoint registry: maps "path:method" to handler functions
    std::unordered_map<std::string, HttpHandler> _endpoints;
    
    // Module factory registry: stores factory functions for auto-registration
    std::vector<std::function<std::unique_ptr<IApiModule>()>> _moduleFactories;
    
    // Initialization flag
    bool _initialized = false;
    
    /**
     * Create endpoint key from path and method
     * @param path The endpoint path
     * @param method The HTTP method
     * @return String key in format "path:method"
     */
    std::string createEndpointKey(std::string_view path, std::string_view method) const;
};

} // namespace apirouter
