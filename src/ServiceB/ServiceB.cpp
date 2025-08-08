#include <IoCContainer/IoCContainer.h>
#include <ServiceB/ServiceB.h>

namespace serviceb {
ServiceB::ServiceB() = default;
std::string ServiceB::doSomethingServiceB() const {
    return "ServiceB result";
}

namespace {
struct ServiceBStaticRegistration {
    ServiceBStaticRegistration() {
        IoCContainer::registerGlobal<ServiceB>([]() { return std::make_shared<ServiceB>(); });
    }
};
static ServiceBStaticRegistration _servicebStaticRegistration;
}

void ServiceB::registerWith(IoCContainer& container) {
    container.registerType<ServiceB>([]() { return std::make_shared<ServiceB>(); });
}
} // namespace serviceb
