#pragma once
#include <ApiModule/IEndpointRegistrar.h>

namespace apimodule {
    /**
     * Interface for API modules to register their endpoints.
     */
    class IApiModule {
    public:
        virtual ~IApiModule() = default;
        virtual void registerEndpoints(IEndpointRegistrar& registrar) = 0;
    };
}
