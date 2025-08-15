#pragma once
#include <ApiRouter/IEndpointRegistrar.h>
#include <string>
#include <string_view>

namespace endpointhello {

/**
 * EndpointHello - HTTP endpoint handler
 * Handles HTTP requests for /hello resources
 * Automatically registers with ApiRouter for request routing
 */
class EndpointHello : public apirouter::IApiModule {
public:
    void registerEndpoints(apirouter::IEndpointRegistrar& registrar) override;

protected:
    // HTTP handler methods (testable protected methods - accessible to unit tests)
    void handleGetHello(std::string_view path, std::string_view method, 
                                 const std::string& requestBody, std::string& responseBody, int& statusCode);
    
    // TODO: Add more HTTP handler methods as needed
    // void handlePostHello(std::string_view path, std::string_view method, 
    //                                const std::string& requestBody, std::string& responseBody, int& statusCode);
    // void handlePutHello(std::string_view path, std::string_view method, 
    //                               const std::string& requestBody, std::string& responseBody, int& statusCode);
    // void handleDeleteHello(std::string_view path, std::string_view method, 
    //                                  const std::string& requestBody, std::string& responseBody, int& statusCode);
};

}
