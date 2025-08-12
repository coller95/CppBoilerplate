#pragma once

// Compatibility header for ApiModule - redirects to ApiRouter
// This ensures that existing endpoint scripts continue to work
#include <ApiRouter/IApiRouter.h>

namespace apimodule {
    using IEndpointRegistrar = apirouter::IEndpointRegistrar;
}
