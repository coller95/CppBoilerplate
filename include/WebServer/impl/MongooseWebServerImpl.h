#pragma once
#include <WebServer/impl/IWebServerImpl.h>
#include <mongoose.h>
#include <map>
#include <string>
#include <string_view>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>

namespace WebServerLib {

class MongooseWebServerImpl : public IWebServerImpl {
public:
        MongooseWebServerImpl(const std::string& ipAddr, uint16_t port);
    ~MongooseWebServerImpl() override;

    void registerHttpHandler(std::string_view path, std::string_view method, WebServer::HttpHandler handler) override;
    void serveStatic(std::string_view urlPrefix, std::string_view directory) override;
    void registerWebSocketHandler(std::string_view path) override;
    void start() override;
    void stop() override;
    bool isRunning() const override;
    void registerPreServeHandler(WebServer::PreServeHandler handler) override;

    /**
     * Serves static files from a directory with a specified MIME type.
     * @param urlPrefix The URL prefix (e.g., "/static/")
     * @param directory The local directory to serve
     * @param mimeType The MIME type to use for the files
     */
    void serveStaticWithMime(std::string_view urlPrefix, std::string_view directory, std::string_view mimeType);

private:
    struct HandlerKey {
        std::string path;
        std::string method;
        bool operator<(const HandlerKey& other) const {
            return std::tie(path, method) < std::tie(other.path, other.method);
        }
    };

    struct StaticMapping {
        std::string urlPrefix;
        std::string directory;
        std::string mimeType; // Added to support custom MIME types
    };

        std::string _ipAddr;
        uint16_t _port;
    std::atomic<bool> _running;
    std::unique_ptr<std::thread> _thread;
    struct mg_mgr _mgr;
    struct mg_connection* _listener;
    std::map<HandlerKey, WebServer::HttpHandler> _handlers;
    std::vector<StaticMapping> _staticMappings;
    std::mutex _mutex;
    WebServer::PreServeHandler _preServeHandler;

    void eventLoop();
    void handleEvent(struct mg_connection* c, int ev, void* ev_data);
};

} // namespace WebServerLib
