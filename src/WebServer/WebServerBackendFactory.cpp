#include <WebServer/WebServerBackendFactory.h>
#include <WebServerBackendMongoose/WebServerBackendMongoose.h>
#include <WebServerBackendPlaceholder/WebServerBackendPlaceholder.h>
#include <stdexcept>

namespace webserver {

std::unique_ptr<IWebServerBackend> WebServerBackendFactory::createBackend(
    Backend backend, 
    const std::string& bindAddress, 
    uint16_t port) {
    
    switch (backend) {
        case Backend::Mongoose:
            return std::make_unique<webserverbackendmongoose::WebServerBackendMongoose>(
                bindAddress, port);
        
        case Backend::Placeholder:
            return std::make_unique<webserverbackendplaceholder::WebServerBackendPlaceholder>(
                bindAddress, port);
        
        default:
            throw std::invalid_argument("Unsupported WebServer backend");
    }
}

std::string WebServerBackendFactory::getBackendName(Backend backend) {
    switch (backend) {
        case Backend::Mongoose: 
            return "Mongoose";
        case Backend::Placeholder:
            return "Placeholder";
        default: 
            return "Unknown";
    }
}

std::vector<WebServerBackendFactory::Backend> WebServerBackendFactory::getAvailableBackends() {
    return {Backend::Mongoose, Backend::Placeholder};
}

} // namespace webserver
