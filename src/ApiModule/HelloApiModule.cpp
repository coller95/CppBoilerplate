#include <ApiModule/HelloApiModule.h>

#include <ApiModule/IEndpointRegistrar.h>
#include <string_view>
#include <string>

namespace apimodule {

void HelloApiModule::registerEndpoints(IEndpointRegistrar& registrar) {
    registrar.registerHttpHandler("/hello", "GET",
        [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
            statusCode = 200;
            responseBody = "Hello from ApiModule!\n";
        });
}

} // namespace apimodule
