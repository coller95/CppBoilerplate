#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <memory>
#include <vector>

namespace apirouter {

// Forward declarations
class IApiModule;
class IEndpointRegistrar;

/**
 * HTTP handler function signature
 * @param path The request path
 * @param method The HTTP method
 * @param requestBody The request body content
 * @param responseBody Output parameter for response body
 * @param statusCode Output parameter for HTTP status code
 */
using HttpHandler = std::function<void(std::string_view path, std::string_view method, const std::string& requestBody, std::string& responseBody, int& statusCode)>;

/**
 * Interface for endpoint registrar - allows modules to register their endpoints
 */
class IEndpointRegistrar {
public:
    virtual ~IEndpointRegistrar() = default;
    
    /**
     * Register an HTTP handler for a specific path and method
     * @param path The endpoint path (e.g., "/api/users")
     * @param method The HTTP method (e.g., "GET", "POST")
     * @param handler The handler function to process requests
     */
    virtual void registerHttpHandler(std::string_view path, std::string_view method, HttpHandler handler) = 0;
};

/**
 * Interface for API modules - endpoints implement this to register themselves
 */
class IApiModule {
public:
    virtual ~IApiModule() = default;
    
    /**
     * Register all endpoints provided by this module
     * @param registrar The registrar to use for endpoint registration
     */
    virtual void registerEndpoints(IEndpointRegistrar& registrar) = 0;
};

/**
 * Interface for ApiRouter - manages endpoint registration and routing
 */
class IApiRouter {
public:
    virtual ~IApiRouter() = default;
    
    /**
     * Initialize the router and auto-register all endpoint modules
     * @return True if initialization was successful
     */
    virtual bool initialize() = 0;
    
    /**
     * Handle an incoming HTTP request
     * @param path The request path
     * @param method The HTTP method
     * @param requestBody The request body content
     * @param responseBody Output parameter for response body
     * @param statusCode Output parameter for HTTP status code
     * @return True if the request was handled
     */
    virtual bool handleRequest(std::string_view path, std::string_view method, const std::string& requestBody, std::string& responseBody, int& statusCode) = 0;
    
    /**
     * Get the number of registered endpoints
     * @return Number of registered endpoints
     */
    virtual size_t getEndpointCount() const = 0;
    
    /**
     * Get a list of all registered endpoints
     * @return Vector of endpoint descriptions (path:method format)
     */
    virtual std::vector<std::string> getRegisteredEndpoints() const = 0;
    
    /**
     * Register a module factory function (used by auto-registration)
     * @param factory Function that creates a new module instance
     */
    virtual void registerModuleFactory(std::function<std::unique_ptr<IApiModule>()> factory) = 0;
    
    /**
     * Get the number of registered module factories
     * @return Number of registered module factories
     */
    virtual size_t getRegisteredModuleCount() const = 0;
    
    /**
     * Create instances of all registered modules
     * @return Vector of created module instances
     */
    virtual std::vector<std::unique_ptr<IApiModule>> createAllModules() const = 0;
};

} // namespace apirouter
