#include "RestfulServer/MongooseHttpServerBackend.h"
#include "mongoose.h"
#include <stdexcept>
#include <utility>

MongooseHttpServerBackend::MongooseHttpServerBackend(int port) : _port(port) {}

MongooseHttpServerBackend::~MongooseHttpServerBackend() {
    if (_running) {
        stop();
    }
}

void MongooseHttpServerBackend::registerHandler(const std::string& path, HandlerFunc handler) {
    _handlers[path] = std::move(handler);
}

void MongooseHttpServerBackend::start() {
    if (_running) throw std::runtime_error("Server is already running");
    _running = true;
    _mgr = new mg_mgr;
    mg_mgr_init(_mgr);
    std::string listenAddr = "0.0.0.0:" + std::to_string(_port);
    struct mg_connection* listener = mg_http_listen(_mgr, listenAddr.c_str(), &MongooseHttpServerBackend::handleEvent, this);
    if (!listener) {
        mg_mgr_free(_mgr);
        delete _mgr;
        _mgr = nullptr;
        _running = false;
        throw std::runtime_error("Failed to start HTTP listener on port " + std::to_string(_port));
    }
    listener->fn_data = this;
    _thread = std::thread([this]() { eventLoop(); });
}

void MongooseHttpServerBackend::stop() {
    if (!_running) throw std::runtime_error("Server is not running");
    _running = false;
    _cv.notify_all();
    if (_thread.joinable()) {
        _thread.join();
    }
    if (_mgr) {
        mg_mgr_free(_mgr);
        delete _mgr;
        _mgr = nullptr;
    }
}

bool MongooseHttpServerBackend::isRunning() const {
    return _running;
}

void MongooseHttpServerBackend::eventLoop() {
    while (_running) {
        mg_mgr_poll(_mgr, 100);
    }
}

void MongooseHttpServerBackend::handleEvent(struct mg_connection* c, int ev, void* ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        auto* backend = static_cast<MongooseHttpServerBackend*>(c->fn_data);
        backend->handleHttpRequest(c, static_cast<struct mg_http_message*>(ev_data));
    }
}

void MongooseHttpServerBackend::handleHttpRequest(struct mg_connection* c, struct mg_http_message* hm) {
    RestfulRequest req;
    req.method = std::string(hm->method.buf, hm->method.len);
    req.path = std::string(hm->uri.buf, hm->uri.len);
    req.body = std::string(hm->body.buf, hm->body.len);
    for (size_t i = 0; i < MG_MAX_HTTP_HEADERS && hm->headers[i].name.len > 0; ++i) {
        req.headers[std::string(hm->headers[i].name.buf, hm->headers[i].name.len)] =
            std::string(hm->headers[i].value.buf, hm->headers[i].value.len);
    }
    auto it = _handlers.find(req.path);
    RestfulResponse resp;
    if (it != _handlers.end()) {
        try {
            it->second(req, resp);
        } catch (const std::exception& ex) {
            resp.statusCode = 500;
            resp.body = std::string("Internal Server Error: ") + ex.what();
        }
    } else {
        resp.statusCode = 404;
        resp.body = "Not Found";
    }
    std::string statusLine = "HTTP/1.1 " + std::to_string(resp.statusCode) + " ";
    switch (resp.statusCode) {
        case 200: statusLine += "OK"; break;
        case 404: statusLine += "Not Found"; break;
        case 500: statusLine += "Internal Server Error"; break;
        default: statusLine += ""; break;
    }
    std::string responseText = statusLine + "\r\n";
    for (const auto& h : resp.headers) {
        responseText += h.first + ": " + h.second + "\r\n";
    }
    if (resp.headers.find("Content-Length") == resp.headers.end()) {
        responseText += "Content-Length: " + std::to_string(resp.body.size()) + "\r\n";
    }
    responseText += "\r\n";
    responseText += resp.body;
    mg_send(c, responseText.data(), responseText.size());
    c->is_draining = 1;
}
