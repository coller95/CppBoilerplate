#pragma once
#include <string>
#include <functional>
#include "RestfulServer/RestfulRequest.h"
#include "RestfulServer/RestfulResponse.h"

using HandlerFunc = std::function<void(const RestfulRequest&, RestfulResponse&)>;

/**
 * Abstract interface for HTTP server backend implementations.
 */
class IHttpServerBackend {
public:
    virtual ~IHttpServerBackend() = default;
    virtual void registerHandler(const std::string& path, HandlerFunc handler) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};
