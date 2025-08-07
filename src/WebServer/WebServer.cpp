#include <WebServer/WebServer.h>
#include <stdexcept>
#include <iostream>

class WebServer::Impl {
public:
    Impl(uint16_t port) : _port(port), _running(false) {}
    void start() {
        if (_running) throw std::runtime_error("WebServer already running");
        _running = true;
        std::cout << "WebServer started on port " << _port << std::endl;
    }
    void stop() {
        if (!_running) return;
        _running = false;
        std::cout << "WebServer stopped" << std::endl;
    }
    bool isRunning() const { return _running; }
    // Stub methods for handlers
    void registerHttpHandler(std::string_view, std::string_view, WebServer::HttpHandler) {}
    void serveStatic(std::string_view, std::string_view) {}
    void registerWebSocketHandler(std::string_view) {}
private:
    uint16_t _port;
    bool _running;
};

WebServer::WebServer(uint16_t port) : _impl(std::make_unique<Impl>(port)) {}
WebServer::~WebServer() = default;
void WebServer::registerHttpHandler(std::string_view path, std::string_view method, HttpHandler handler) { _impl->registerHttpHandler(path, method, std::move(handler)); }
void WebServer::serveStatic(std::string_view urlPrefix, std::string_view directory) { _impl->serveStatic(urlPrefix, directory); }
void WebServer::registerWebSocketHandler(std::string_view path) { _impl->registerWebSocketHandler(path); }
void WebServer::start() { _impl->start(); }
void WebServer::stop() { _impl->stop(); }
bool WebServer::isRunning() const { return _impl->isRunning(); }
