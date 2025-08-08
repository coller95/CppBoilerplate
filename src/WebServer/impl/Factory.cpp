#include <WebServer/impl/Factory.h>
#include <WebServer/impl/MongooseWebServerImpl.h>

namespace WebServerLib {
std::unique_ptr<IWebServerImpl> createDefaultWebServerImpl(const std::string& ipAddr, uint16_t port) {
    return std::make_unique<MongooseWebServerImpl>(ipAddr, port);
}

std::unique_ptr<IWebServerImpl> createWebServerImpl(const std::string& ipAddr, uint16_t port, int backendEnum) {
    switch (backendEnum) {
        case 0: // Mongoose
        default:
            return std::make_unique<MongooseWebServerImpl>(ipAddr, port);
        case 1: 
            return std::make_unique<MongooseWebServerImpl>(ipAddr, port);
    }
}
}
