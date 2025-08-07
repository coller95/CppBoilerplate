
#include "RestfulServer/RestfulServer.h"


#include <stdexcept>
#include <utility>
#include <memory>
#include <string>
#include <vector>
#include "RestfulServer/RestfulRequest.h"
#include "RestfulServer/RestfulResponse.h"
#include "RestfulServer/IHttpServerBackend.h"
#include "RestfulServer/MongooseHttpServerBackend.h"

// ...existing code...

// --- RestfulServer Implementation ---
class RestfulServer::Impl {
public:
    explicit Impl(int port)
        : _backend(std::make_unique<MongooseHttpServerBackend>(port)) {}

    void registerHandler(const std::string& path, HandlerFunc handler) {
        _backend->registerHandler(path, std::move(handler));
    }
    void start() { _backend->start(); }
    void stop() { _backend->stop(); }
    bool isRunning() const { return _backend->isRunning(); }
private:
    std::unique_ptr<IHttpServerBackend> _backend;
};

RestfulServer::RestfulServer(int port)
    : _impl(std::make_unique<Impl>(port)) {}

RestfulServer::~RestfulServer() = default;

void RestfulServer::registerHandler(const std::string& path, HandlerFunc handler) {
    _impl->registerHandler(path, std::move(handler));
}

void RestfulServer::start() {
    _impl->start();
}

void RestfulServer::stop() {
    _impl->stop();
}

bool RestfulServer::isRunning() const {
    return _impl->isRunning();
}
