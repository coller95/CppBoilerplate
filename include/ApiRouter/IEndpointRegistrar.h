#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <memory>
#include <vector>

namespace apirouter {

// Forward declaration
class IApiModule;

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

// IApiRouter interface removed - ApiRouter is now a concrete singleton
// All functionality moved directly to ApiRouter class

} // namespace apirouter
