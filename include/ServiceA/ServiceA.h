#pragma once
#include <string>
#include <IoCContainer/IoCContainer.h>

namespace servicea {
class ServiceA {
public:
    ServiceA();
    std::string doSomethingServiceA() const;
    // Static registration helper for TDD and decoupling
    static void registerWith(IoCContainer& container);
};
}
