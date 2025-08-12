#pragma once

#include <WebServer/IWebServerBackend.h>

namespace webserverbackendmongoose {

/**
 * Interface for WebServerBackendMongoose
 * Extends IWebServerBackend with Mongoose-specific functionality
 */
class IWebServerBackendMongoose : public webserver::IWebServerBackend {
public:
    virtual ~IWebServerBackendMongoose() = default;
    
    // Mongoose-specific configuration methods
    virtual void setDocumentRoot(const std::string& documentRoot) = 0;
    virtual void setMaxRequestSize(size_t maxSize) = 0;
    virtual void setConnectionTimeout(int timeoutSeconds) = 0;
    virtual bool enableSSL(const std::string& certFile, const std::string& keyFile) = 0;
};

} // namespace webserverbackendmongoose
