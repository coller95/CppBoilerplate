#include <gtest/gtest.h>
#include <IocContainer/IocContainer.h>
#include <Logger/ILogger.h>
#include <Logger/Logger.h>

namespace {

class IocContainerRegistryListingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clear container for clean test state
        auto& container = ioccontainer::IocContainer::getInstance();
        container.clear();
    }

    void TearDown() override {
        // Cleanup after each test
        auto& container = ioccontainer::IocContainer::getInstance();
        container.clear();
    }
};

} // anonymous namespace

TEST_F(IocContainerRegistryListingTest, CanGetRegisteredTypeNames) {
    auto& container = ioccontainer::IocContainer::getInstance();
    
    // Register multiple services
    auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    container.registerInstance<logger::ILogger>(logger);
    
    // Get list of registered type names
    auto typeNames = container.getRegisteredTypeNames();
    
    // Should have exactly one registration
    EXPECT_EQ(1U, typeNames.size());
    
    // Should contain the logger interface type name
    EXPECT_FALSE(typeNames.empty());
    
    // The type name should contain "ILogger" (though exact format may vary by compiler)
    bool foundLogger = false;
    for (const auto& typeName : typeNames) {
        if (typeName.find("ILogger") != std::string::npos) {
            foundLogger = true;
            break;
        }
    }
    EXPECT_TRUE(foundLogger);
}

TEST_F(IocContainerRegistryListingTest, EmptyContainerReturnsEmptyList) {
    auto& container = ioccontainer::IocContainer::getInstance();
    
    // Get list from empty container
    auto typeNames = container.getRegisteredTypeNames();
    
    // Should be empty
    EXPECT_TRUE(typeNames.empty());
    EXPECT_EQ(0U, typeNames.size());
}

TEST_F(IocContainerRegistryListingTest, CanGetRegisteredServicesInfo) {
    auto& container = ioccontainer::IocContainer::getInstance();
    
    // Register a service
    auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    container.registerInstance<logger::ILogger>(logger);
    
    // Get services info as string
    auto servicesInfo = container.getRegisteredServicesInfo();
    EXPECT_FALSE(servicesInfo.empty());
    EXPECT_TRUE(servicesInfo.find("1 service") != std::string::npos);
    EXPECT_TRUE(servicesInfo.find("Logger Interface") != std::string::npos);
}

TEST_F(IocContainerRegistryListingTest, EmptyContainerReturnsCorrectInfo) {
    auto& container = ioccontainer::IocContainer::getInstance();
    
    // Get info from empty container
    auto servicesInfo = container.getRegisteredServicesInfo();
    EXPECT_FALSE(servicesInfo.empty());
    EXPECT_TRUE(servicesInfo.find("No services registered") != std::string::npos);
}

TEST_F(IocContainerRegistryListingTest, RegistryDataConsistency) {
    auto& container = ioccontainer::IocContainer::getInstance();
    
    // Register a service
    auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    container.registerInstance<logger::ILogger>(logger);
    
    // All data access methods should be consistent
    auto typeNames = container.getRegisteredTypeNames();
    auto servicesInfo = container.getRegisteredServicesInfo();
    auto count = container.getRegisteredCount();
    
    // Verify consistency
    EXPECT_EQ(1U, count);
    EXPECT_EQ(1U, typeNames.size());
    EXPECT_TRUE(servicesInfo.find("1 service") != std::string::npos);
}
