#pragma once
#include <ApiModule/IEndpointRegistrar.h>

namespace apimodule {
    /**
     * Static class to register all API modules with the registrar.
     */
    class ApiModules {
    public:
        static void registerAll(IEndpointRegistrar& registrar);
    };
}
