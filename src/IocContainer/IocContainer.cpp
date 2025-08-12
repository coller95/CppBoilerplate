#include <IocContainer/IocContainer.h>

namespace ioccontainer {

IocContainer& IocContainer::getInstance() {
    // Meyer's singleton: thread-safe in C++11 and later
    // The static local variable is initialized only once,
    // even in multi-threaded environments
    static IocContainer instance;
    return instance;
}

IocContainer::IocContainer() {
    // Private constructor for singleton pattern
    // TODO: Initialize your IoC container here
}

IocContainer::~IocContainer() {
    // TODO: Cleanup resources here
    // Note: If using PIMPL, destructor must be defined in .cpp file
}

// TODO: Implement your public methods here
// Example:
// bool IocContainer::initialize() {
//     // Implementation here
//     return true;
// }

} // namespace ioccontainer
