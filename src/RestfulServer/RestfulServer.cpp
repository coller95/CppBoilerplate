#include "RestfulServer/RestfulServer.h"
#include <stdexcept>
#include <utility>

class RestfulServer::Impl {
public:
    explicit Impl(int port)
        : _port(port), _running(false) {}

    void registerHandler(const std::string& path, HandlerFunc handler) {
        _handlers[path] = std::move(handler);
    }

    void start() {
        if (_running) {
            throw std::runtime_error("Server is already running");
        }
        // Placeholder: actual server logic would go here
        _running = true;
    }

    void stop() {
        if (!_running) {
            throw std::runtime_error("Server is not running");
        }
        // Placeholder: actual shutdown logic would go here
        _running = false;
    }

    bool isRunning() const {
        return _running;
    }

private:
    int _port;
    bool _running;
    std::unordered_map<std::string, HandlerFunc> _handlers;
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
