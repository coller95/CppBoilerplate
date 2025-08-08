#pragma once
#include <ApiModule/IApiModule.h>

namespace apimodule {
    class HelloApiModule : public IApiModule {
    public:
        void registerEndpoints(IEndpointRegistrar& registrar) override;
    };
}
