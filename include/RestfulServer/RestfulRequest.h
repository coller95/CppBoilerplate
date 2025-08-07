#pragma once
#include <string>
#include <unordered_map>

/**
 * Represents an HTTP request received by the server.
 */
class RestfulRequest {
public:
    std::string method;
    std::string path;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    // Add more fields as needed (query params, etc.)
};
