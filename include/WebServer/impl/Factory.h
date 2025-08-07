#pragma once
#include <WebServer/impl/IWebServerImpl.h>
// Add new backend includes here as needed

namespace WebServerLib {
// Factory for creating the default web server implementation
std::unique_ptr<IWebServerImpl> createDefaultWebServerImpl(uint16_t port);
}
