#pragma once

#include <gmock/gmock.h>
#include <IocContainer/IIocContainer.h>

namespace ioccontainer {

/**
 * Mock implementation of IIocContainer for testing
 */
class MockIocContainer : public IIocContainer {
public:
    // TODO: Add mock methods for your interface
    // Example:
    // MOCK_METHOD(bool, initialize, (), (override));
    // MOCK_METHOD(void, cleanup, (), (override));
};

} // namespace ioccontainer
