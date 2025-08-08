
#include <ApiModule/ApiModules.h>
#include <ApiModule/IApiModule.h>
#include <EndpointHello/EndpointHello.h>

namespace apimodule {

void ApiModules::registerAll(IEndpointRegistrar& registrar) {
    // Register all API modules here
    endpointhello::EndpointHello helloEndpoint;
    helloEndpoint.registerEndpoints(registrar);
}

} // namespace apimodule
