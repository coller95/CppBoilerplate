#pragma once
#include <ApiModule/IApiModule.h>

namespace endpointhello {
    class EndpointHello : public apimodule::IApiModule {
    public:
        void registerEndpoints(IEndpointRegistrar& registrar) override;
    };
}
