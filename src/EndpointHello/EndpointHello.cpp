#include <EndpointHello/EndpointHello.h>
#include <ApiRouter/ApiRouter.h>
#include <string_view>
#include <string>
#include <memory>

namespace endpointhello {

void EndpointHello::registerEndpoints(apirouter::IEndpointRegistrar& registrar) {
    registrar.registerHttpHandler("/hello", "GET",
       [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
           statusCode = 200;
           responseBody = "Hello from EndpointHello!\n";
       });
}

// Auto-registration: Register this endpoint module with ApiRouter
namespace {
    static bool registered = []() {
        apirouter::ApiRouter::registerModuleFactoryGlobal([]() -> std::unique_ptr<apirouter::IApiModule> {
            return std::make_unique<EndpointHello>();
        });
        return true;
    }();
}

} // namespace endpointhello
