#include <ApiModule/ApiModules.h>

#include <ApiModule/IApiModule.h>
#include <ApiModule/HelloApiModule.h>

namespace apimodule {

void ApiModules::registerAll(IEndpointRegistrar& registrar) {
    // Register all API modules here
    HelloApiModule helloModule;
    helloModule.registerEndpoints(registrar);
}

} // namespace apimodule
