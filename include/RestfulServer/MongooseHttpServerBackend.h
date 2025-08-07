#pragma once
#include "RestfulServer/IHttpServerBackend.h"
#include <unordered_map>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <mutex>

// Forward declaration for mongoose types
struct mg_connection;
struct mg_http_message;
struct mg_mgr;

/**
 * Mongoose-based HTTP server backend implementation.
 */
class MongooseHttpServerBackend : public IHttpServerBackend {
public:
    explicit MongooseHttpServerBackend(int port);
    ~MongooseHttpServerBackend() override;
    void registerHandler(const std::string& path, HandlerFunc handler) override;
    void start() override;
    void stop() override;
    bool isRunning() const override;
private:
    static void handleEvent(struct mg_connection* c, int ev, void* ev_data);
    void eventLoop();
    void handleHttpRequest(struct mg_connection* c, struct mg_http_message* hm);

    int _port;
    std::atomic<bool> _running{false};
    std::unordered_map<std::string, HandlerFunc> _handlers;
    struct mg_mgr* _mgr = nullptr;
    std::thread _thread;
    std::condition_variable _cv;
    std::mutex _mutex;
};
