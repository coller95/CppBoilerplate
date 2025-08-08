#pragma once
#include <string_view>
#include <functional>

class IEndpointRegistrar {
public:
    using HttpHandler = std::function<void(std::string_view path, std::string_view method, const std::string& body, std::string& responseBody, int& statusCode)>;
    virtual ~IEndpointRegistrar() = default;
    virtual void registerHttpHandler(std::string_view path, std::string_view method, HttpHandler handler) = 0;
};
