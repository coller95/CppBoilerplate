#include <WebServer/WebServer.h>
#include <WebServer/impl/Factory.h>
#include <memory>

using namespace WebServerLib;

class WebServer::Impl {
public:
    explicit Impl(uint16_t port)
        : _impl(WebServerLib::createDefaultWebServerImpl(port)) {}
    void registerHttpHandler(std::string_view path, std::string_view method, WebServer::HttpHandler handler) {
        _impl->registerHttpHandler(path, method, std::move(handler));
    }
    void serveStatic(std::string_view urlPrefix, std::string_view directory) {
        _impl->serveStatic(urlPrefix, directory);
    }
    void registerWebSocketHandler(std::string_view path) {
        _impl->registerWebSocketHandler(path);
    }
    void start() { _impl->start(); }
    void stop() { _impl->stop(); }
    bool isRunning() const { return _impl->isRunning(); }
    void serveStaticWithMime(std::string_view urlPrefix, std::string_view directory, std::string_view mimeType) {
        _impl->serveStaticWithMime(urlPrefix, directory, mimeType);
    }
    void registerPreServeHandler(WebServer::PreServeHandler handler) {
        _impl->registerPreServeHandler(std::move(handler));
    }
private:
    std::unique_ptr<IWebServerImpl> _impl;
};

WebServer::WebServer(uint16_t port) : _impl(std::make_unique<Impl>(port)) {}
WebServer::~WebServer() = default;
void WebServer::registerHttpHandler(std::string_view path, std::string_view method, HttpHandler handler) { _impl->registerHttpHandler(path, method, std::move(handler)); }
void WebServer::serveStatic(std::string_view urlPrefix, std::string_view directory) { _impl->serveStatic(urlPrefix, directory); }
void WebServer::registerWebSocketHandler(std::string_view path) { _impl->registerWebSocketHandler(path); }
void WebServer::start() { _impl->start(); }
void WebServer::stop() { _impl->stop(); }
bool WebServer::isRunning() const { return _impl->isRunning(); }
void WebServer::serveStaticWithMime(std::string_view urlPrefix, std::string_view directory, std::string_view mimeType) {
    _impl->serveStaticWithMime(urlPrefix, directory, mimeType);
}
void WebServer::registerPreServeHandler(PreServeHandler handler) {
    _impl->registerPreServeHandler(std::move(handler));
}
