#include <WebServer/impl/Factory.h>
#include <WebServer/impl/MongooseWebServerImpl.h>

namespace WebServerLib {
std::unique_ptr<IWebServerImpl> createDefaultWebServerImpl(const std::string& ipAddr, uint16_t port) {
    return std::make_unique<MongooseWebServerImpl>(ipAddr, port);
}
}
