#include <EndpointHello/EndpointHello.h>
#include <ApiModule/IEndpointRegistrar.h>
#include <ApiModule/ApiModule.h>
#include <string_view>
#include <string>
#include <memory>

namespace endpointhello {

void EndpointHello::registerEndpoints(IEndpointRegistrar& registrar) {
    registrar.registerHttpHandler("/hello", "GET",
       [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
           statusCode = 200;
           responseBody = "Hello from EndpointHello!\n";
       });
}

// Auto-registration: Register this endpoint module with ApiModule
namespace {
    static bool registered = []() {
        apimodule::ApiModule::registerModuleFactory([]() -> std::unique_ptr<apimodule::IApiModule> {
            return std::make_unique<EndpointHello>();
        });
        return true;
    }();
}

} // namespace endpointhello
