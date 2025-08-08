#pragma once
#include <string>
#include <IoCContainer/IoCContainer.h>

namespace serviceb {
class ServiceB {
public:
    ServiceB();
    std::string doSomethingServiceB() const;
    // Static registration helper for TDD and decoupling
    static void registerWith(IoCContainer& container);
};
}
