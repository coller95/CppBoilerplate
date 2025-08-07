
#pragma once


#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include "RestfulServer/RestfulRequest.h"
#include "RestfulServer/RestfulResponse.h"

/**
 * Provides a simple RESTful HTTP server interface for handling requests and responses.
 */
class RestfulServer {
public:
    using HandlerFunc = std::function<void(const RestfulRequest&, RestfulResponse&)>;

    /**
     * Constructs a RestfulServer listening on the specified port.
     * @param port The port number to listen on
     */
    explicit RestfulServer(int port);

    /**
     * Registers a handler for a specific HTTP path.
     * @param path The URL path to handle (e.g., "/api/hello")
     * @param handler The function to handle requests to this path
     */
    void registerHandler(const std::string& path, HandlerFunc handler);

    /**
     * Starts the server and begins listening for requests.
     */
    void start();

    /**
     * Stops the server if running.
     */
    void stop();

    /**
     * Checks if the server is currently running.
     * @return true if running, false otherwise
     */
    bool isRunning() const;

    /**
     * Destructor
     */
    ~RestfulServer();

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};
