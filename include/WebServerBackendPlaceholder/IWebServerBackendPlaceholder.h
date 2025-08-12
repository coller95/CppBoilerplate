#pragma once

#include <WebServer/IWebServerBackend.h>

namespace webserverbackendplaceholder {

/**
 * Interface for WebServerBackendPlaceholder
 * 
 * A reference implementation of IWebServerBackend for testing and development.
 * This backend provides a minimal, in-memory implementation that doesn't 
 * actually bind to network ports, making it perfect for unit testing.
 */
class IWebServerBackendPlaceholder : public webserver::IWebServerBackend {
public:
    virtual ~IWebServerBackendPlaceholder() = default;
    
    // Additional placeholder-specific methods (if needed)
    virtual void setSimulatedLatency(int milliseconds) = 0;
    virtual size_t getHandlerCount() const = 0;
    virtual void clearAllHandlers() = 0;
};

} // namespace webserverbackendplaceholder
