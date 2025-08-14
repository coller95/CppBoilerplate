#pragma once

#include <gmock/gmock.h>
#include <IocContainer/IIocContainer.h>

namespace ioccontainer {

/**
 * Mock implementation of IIocContainer for testing
 */
class MockIocContainer : public IIocContainer {
private:
    // Mock the virtual implementation methods (for template delegation)
    MOCK_METHOD(void, registerService_impl, (const std::type_info& type, std::shared_ptr<void> instance), (override));
    MOCK_METHOD(std::shared_ptr<void>, resolveService_impl, (const std::type_info& type), (override));
    MOCK_METHOD(bool, isRegistered_impl, (const std::type_info& type), (const, override));

public:
    // Mock methods for the IIocContainer interface
    MOCK_METHOD(bool, isRegistered, (const std::type_info& type), (const, override));
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(size_t, getRegisteredCount, (), (const, override));
    MOCK_METHOD(std::vector<std::string>, getRegisteredTypeNames, (), (const, override));
    MOCK_METHOD(std::string, getRegisteredServicesInfo, (), (const, override));
};

} // namespace ioccontainer
