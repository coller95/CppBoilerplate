#include <WebServer/impl/MongooseWebServerImpl.h>
#include <iostream>
#include <cstring>
#include <fstream>

using namespace WebServerLib;

MongooseWebServerImpl::MongooseWebServerImpl(uint16_t port)
    : _port(port), _running(false), _listener(nullptr) {
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
    _staticMappings.push_back({std::string(urlPrefix), std::string(directory)});
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

void MongooseWebServerImpl::handleEvent(struct mg_connection* c, int ev, void* ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        auto* hm = (struct mg_http_message*)ev_data;
        std::string path(hm->uri.buf, hm->uri.len);
        std::string method(hm->method.buf, hm->method.len);
        std::string body(hm->body.buf, hm->body.len);
        std::string responseBody;
        int statusCode = 200;
        bool handled = false;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            HandlerKey key{path, method};
            auto it = _handlers.find(key);
            if (it != _handlers.end()) {
                it->second(path, method, body, responseBody, statusCode);
                handled = true;
            } else {
                // Check static mappings
                for (const auto& mapping : _staticMappings) {
                    if (path.rfind(mapping.urlPrefix, 0) == 0) {
                        std::string relPath = path.substr(mapping.urlPrefix.length());
                        std::string filePath = mapping.directory;
                        if (!filePath.empty() && filePath.back() != '/' && !relPath.empty() && relPath.front() != '/') {
                            filePath += "/";
                        }
                        filePath += relPath;
                        std::cout << "[MongooseWebServerImpl] Trying to serve file: " << filePath << std::endl;
                        std::ifstream f(filePath);
                        if (f.good()) {
                            std::cout << "[MongooseWebServerImpl] File exists: " << filePath << std::endl;
                        } else {
                            std::cout << "[MongooseWebServerImpl] File NOT FOUND: " << filePath << std::endl;
                        }
                        // Serve .bin files manually as application/octet-stream
                        if (filePath.size() >= 4 && filePath.substr(filePath.size() - 4) == ".bin") {
                            std::ifstream binFile(filePath, std::ios::binary);
                            if (binFile) {
                                binFile.seekg(0, std::ios::end);
                                size_t fileSize = binFile.tellg();
                                std::cout << "[MongooseWebServerImpl] .bin file size: " << fileSize << std::endl;
                                binFile.seekg(0, std::ios::beg);
                                std::vector<char> buffer(fileSize);
                                binFile.read(buffer.data(), fileSize);
                                std::cout << "[MongooseWebServerImpl] .bin file read: " << binFile.gcount() << " bytes" << std::endl;
                                mg_http_reply(c, 200, "Content-Type: application/octet-stream\r\n", "%.*s", (int)fileSize, buffer.data());
                            } else {
                                std::cout << "[MongooseWebServerImpl] .bin file open failed!" << std::endl;
                                mg_http_reply(c, 404, "Content-Type: text/plain\r\n", "Not found");
                            }
                        } else {
                            static const char* binMime = ".bin=application/octet-stream";
                            struct mg_http_serve_opts opts = {
                                mapping.directory.c_str(), // root_dir
                                nullptr, // ssi_pattern
                                nullptr, // extra_headers
                                binMime, // mime_types
                                nullptr, // page404
                                nullptr  // fs
                            };
                            mg_http_serve_file(c, hm, filePath.c_str(), &opts);
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
