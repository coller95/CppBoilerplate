#pragma once

#include <memory>
#include <string>
#include <vector>
#include <WebServer/IWebServer.h>

namespace webserver {

class WebServerBackendFactory {
public:
    enum class Backend {
        Mongoose,
        Placeholder
        // Future backends can be added here
    };
    
    static std::unique_ptr<IWebServer> createBackend(
        Backend backend, 
        const std::string& bindAddress, 
        uint16_t port
    );
    
    static std::string getBackendName(Backend backend);
    static std::vector<Backend> getAvailableBackends();
};

} // namespace webserver
