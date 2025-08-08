#include <IoCContainer/IoCContainer.h>
#include <ServiceA/ServiceA.h>

namespace servicea {
ServiceA::ServiceA() = default;
std::string ServiceA::doSomethingServiceA() const {
    return "ServiceA result";
}

namespace {
struct ServiceAStaticRegistration {
    ServiceAStaticRegistration() {
        IoCContainer::registerGlobal<ServiceA>([]() { return std::make_shared<ServiceA>(); });
    }
};
static ServiceAStaticRegistration _serviceaStaticRegistration;
}

void ServiceA::registerWith(IoCContainer& container) {
    container.registerType<ServiceA>([]() { return std::make_shared<ServiceA>(); });
}
} // namespace servicea
