#pragma once
#include <string>
#include <unordered_map>

/**
 * Represents an HTTP response to be sent by the server handler.
 */
class RestfulResponse {
public:
    int statusCode = 200;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    void setHeader(const std::string& key, const std::string& value) {
        headers[key] = value;
    }
    void setStatus(int code) {
        statusCode = code;
    }
    void setBody(const std::string& b) {
        body = b;
    }
};
