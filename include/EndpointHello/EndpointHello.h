#pragma once
#include <ApiRouter/IApiRouter.h>

namespace endpointhello {
    class EndpointHello : public apirouter::IApiModule {
    public:
        void registerEndpoints(apirouter::IEndpointRegistrar& registrar) override;
    };
}
