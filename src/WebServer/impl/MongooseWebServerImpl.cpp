
#include <WebServer/impl/MongooseWebServerImpl.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <string>

namespace WebServerLib {

MongooseWebServerImpl::MongooseWebServerImpl(const std::string& ipAddr, uint16_t port)
    : _ipAddr(ipAddr), _port(port), _running(false), _listener(nullptr), _preServeHandler(nullptr) {
    mg_mgr_init(&_mgr);
}

MongooseWebServerImpl::~MongooseWebServerImpl() {
    stop();
    mg_mgr_free(&_mgr);
}

void MongooseWebServerImpl::registerHttpHandler(std::string_view path, std::string_view method, WebServer::HttpHandler handler) {
    std::lock_guard<std::mutex> lock(_mutex);
    HandlerKey key{std::string(path), std::string(method)};
    _handlers[key] = std::move(handler);
}

void MongooseWebServerImpl::serveStatic(std::string_view urlPrefix, std::string_view directory) {
    std::lock_guard<std::mutex> lock(_mutex);
    std::cout << "[serveStatic] Registering static mapping: " << urlPrefix << " -> " << directory << std::endl;
    _staticMappings.push_back({std::string(urlPrefix), std::string(directory), ""});
}

void MongooseWebServerImpl::serveStaticWithMime(std::string_view urlPrefix, std::string_view directory, std::string_view mimeType) {
    std::lock_guard<std::mutex> lock(_mutex);
    _staticMappings.push_back({std::string(urlPrefix), std::string(directory), std::string(mimeType)});
}

void MongooseWebServerImpl::registerWebSocketHandler(std::string_view /*path*/) {
    // Not implemented
}

void MongooseWebServerImpl::start() {
    if (_running) throw std::runtime_error("WebServer already running");
    _running = true;
    std::string listenAddr = "0.0.0.0:" + std::to_string(_port);
    _listener = mg_http_listen(&_mgr, listenAddr.c_str(), [](mg_connection* c, int ev, void* ev_data) {
        auto* self = static_cast<MongooseWebServerImpl*>(c->fn_data);
        if (self) self->handleEvent(c, ev, ev_data);
    }, this);
    if (!_listener) throw std::runtime_error("Failed to start Mongoose listener");
    _thread = std::make_unique<std::thread>(&MongooseWebServerImpl::eventLoop, this);
    std::cout << "MongooseWebServer started on port " << _port << std::endl;
}

void MongooseWebServerImpl::stop() {
    if (!_running) return;
    _running = false;
    mg_wakeup(&_mgr, 0, nullptr, 0);
    if (_thread && _thread->joinable()) _thread->join();
    _thread.reset();
    _listener = nullptr;
    std::cout << "MongooseWebServer stopped" << std::endl;
}

bool MongooseWebServerImpl::isRunning() const {
    return _running;
}

void MongooseWebServerImpl::eventLoop() {
    while (_running) {
        mg_mgr_poll(&_mgr, 100);
    }
}

void MongooseWebServerImpl::registerPreServeHandler(WebServer::PreServeHandler handler) {
    std::lock_guard<std::mutex> lock(_mutex);
    _preServeHandler = std::move(handler);
}

void MongooseWebServerImpl::handleEvent(struct mg_connection* c, int ev, void* ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        auto* hm = (struct mg_http_message*)ev_data;
        std::string path(hm->uri.ptr, hm->uri.len);
        std::cout << "[handleEvent] Received request for path: " << path << std::endl;
        std::string method(hm->method.ptr, hm->method.len);
        std::string body(hm->body.ptr, hm->body.len);
        std::string responseBody;
        int statusCode = 200;
        bool handled = false;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            // Removed early call of _preServeHandler(path) to ensure handler only receives resolved file paths
            HandlerKey key{path, method};
            auto it = _handlers.find(key);
            if (it != _handlers.end()) {
                it->second(path, method, body, responseBody, statusCode);
                handled = true;
            } else {
                // Check static mappings
                for (const auto& mapping : _staticMappings) {
                    if (path.rfind(mapping.urlPrefix, 0) == 0) {
                        std::cout << "[handleEvent] Checking static mapping: " << mapping.urlPrefix << " -> " << mapping.directory << std::endl;
                        std::string relPath = path.substr(mapping.urlPrefix.length());
                        std::string filePath = mapping.directory;
                        if (!filePath.empty() && filePath.back() != '/' && !relPath.empty() && relPath.front() != '/') {
                            filePath += "/";
                        }
                        filePath += relPath;
                        std::cout << "[handleEvent] Constructed file path: " << filePath << std::endl;

                        std::ifstream file(filePath);
                        if (!file) {
                            std::cerr << "[handleEvent] File not found or inaccessible: " << filePath << std::endl;
                            continue;
                        }

                        if (_preServeHandler) {
                            try {
                                _preServeHandler(filePath);
                            } catch (const std::exception& e) {
                                std::cerr << "PreServeHandler exception: " << e.what() << std::endl;
                            }
                        }

                        std::cout << "[handleEvent] Preparing to serve file: " << filePath << std::endl;
                        try {
                            std::cout << "[handleEvent] mg_connection address: " << c << std::endl;
                            std::cout << "[handleEvent] mg_http_message URI: " << std::string(hm->uri.ptr, hm->uri.len) << std::endl;
                            std::cout << "[handleEvent] mg_http_message Method: " << std::string(hm->method.ptr, hm->method.len) << std::endl;
                            const char* binMime = ".bin=application/octet-stream";
                            struct mg_http_serve_opts opts = {
                                /*root_dir*/ mapping.directory.c_str(),
                                /*ssi_pattern*/ nullptr,
                                /*extra_headers*/ nullptr,
                                /*mime_types*/ binMime,
                                /*page404*/ nullptr,
                                /*fs*/ nullptr
                            };
                            mg_http_serve_file(c, hm, filePath.c_str(), &opts);
                            std::cout << "[handleEvent] Successfully served file: " << filePath << std::endl;
                        } catch (const std::exception& e) {
                            std::cerr << "[handleEvent] Exception while serving file: " << e.what() << std::endl;
                        }
                        handled = true;
                        break;
                    }
                }
            }
        }
        if (handled && responseBody.size() > 0) {
            mg_http_reply(c, statusCode, "Content-Type: text/plain\r\n", "%s", responseBody.c_str());
        } else if (!handled) {
            mg_http_reply(c, 404, "Content-Type: text/plain\r\n", "Not found");
        }
    }
}

}