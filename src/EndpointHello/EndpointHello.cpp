#include <EndpointHello/EndpointHello.h>
#include <ApiRouter/ApiRouter.h>
#include <string_view>
#include <string>
#include <memory>

namespace endpointhello {

void EndpointHello::registerEndpoints(apirouter::IEndpointRegistrar& registrar) {
    // Register GET /hello handler using lambda that calls private method
    registrar.registerHttpHandler("/hello", "GET",
        [this](std::string_view path, std::string_view method, const std::string& requestBody, 
                std::string& responseBody, int& statusCode) {
            handleGetHello(path, method, requestBody, responseBody, statusCode);
        });
    
    // TODO: Register additional endpoints as needed
    // registrar.registerHttpHandler("/hello", "POST",
    //     [this](std::string_view path, std::string_view method, const std::string& requestBody, 
    //             std::string& responseBody, int& statusCode) {
    //         handlePostHello(path, method, requestBody, responseBody, statusCode);
    //     });
}

void EndpointHello::handleGetHello(std::string_view /*path*/, std::string_view /*method*/,
                                             const std::string& /*requestBody*/, std::string& responseBody, int& statusCode) {
    // TODO: Implement your GET /hello logic here
    statusCode = 200;
    responseBody = "Hello from EndpointHello! This is the GET /hello endpoint.\n";
    
    // Example of JSON response:
    // responseBody = R"({"message": "Hello from Hello", "status": "success"})";
    
    // Example of error handling:
    // if (some_error_condition) {
    //     statusCode = 400;
    //     responseBody = R"({"error": "Bad request", "message": "Invalid Hello data"})";
    //     return;
    // }
}

// TODO: Implement additional handler methods
// void EndpointHello::handlePostHello(std::string_view path, std::string_view method,
//                                               const std::string& requestBody, std::string& responseBody, int& statusCode) {
//     // TODO: Implement POST logic
//     statusCode = 201;
//     responseBody = R"({"message": "Hello created successfully"})";
// }

// Auto-registration: Register this endpoint module with ApiRouter
namespace {
    struct EndpointHelloRegistration {
        EndpointHelloRegistration() {
            apirouter::ApiRouter::registerModuleFactoryGlobal([]() -> std::unique_ptr<apirouter::IApiModule> {
                return std::make_unique<EndpointHello>();
            });
        }
    };
    static EndpointHelloRegistration _registration;
}

} // namespace endpointhello
