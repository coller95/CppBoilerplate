#pragma once

#include <gmock/gmock.h>
#include <IocContainer/IIocContainer.h>

namespace ioccontainer {

/**
 * Mock implementation of IIocContainer for testing
 */
class MockIocContainer : public IIocContainer {
public:
    // Mock methods for the IIocContainer interface
    MOCK_METHOD(bool, isRegistered, (const std::type_info& type), (const, override));
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(size_t, getRegisteredCount, (), (const, override));
};

} // namespace ioccontainer
