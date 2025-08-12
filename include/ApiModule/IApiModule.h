#pragma once

// Compatibility header for ApiModule - redirects to ApiRouter
// This ensures that existing endpoint scripts continue to work
#include <ApiRouter/IApiRouter.h>

namespace apimodule {
    // Type aliases for compatibility with existing endpoint scripts
    using IApiModule = apirouter::IApiModule;
    using IEndpointRegistrar = apirouter::IEndpointRegistrar;
    using HttpHandler = apirouter::HttpHandler;
}
