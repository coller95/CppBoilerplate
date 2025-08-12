#pragma once

namespace ioccontainer {

/**
 * Interface for IocContainer
 * Define the contract that implementations must follow
 */
class IIocContainer {
public:
    virtual ~IIocContainer() = default;
    
    // TODO: Define your interface methods here
    // Example:
    // virtual bool initialize() = 0;
    // virtual void cleanup() = 0;
};

} // namespace ioccontainer
