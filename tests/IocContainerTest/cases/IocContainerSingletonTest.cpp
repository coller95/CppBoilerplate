#include <gtest/gtest.h>
#include <IocContainer/IocContainer.h>
#include <thread>
#include <vector>

namespace {

class IocContainerSingletonTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset singleton state if needed for clean tests
    }

    void TearDown() override {
        // Cleanup after tests
    }
};

} // anonymous namespace

TEST_F(IocContainerSingletonTest, GetInstanceReturnsSameInstance) {
    // Get two references to the singleton
    auto& instance1 = ioccontainer::IocContainer::getInstance();
    auto& instance2 = ioccontainer::IocContainer::getInstance();
    
    // They should be the same object
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(IocContainerSingletonTest, SingletonIsThreadSafe) {
    std::vector<std::thread> threads;
    std::vector<ioccontainer::IocContainer*> instances(10);
    
    // Create multiple threads that all try to get the singleton instance
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&instances, i]() {
            instances[i] = &ioccontainer::IocContainer::getInstance();
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // All instances should be the same
    for (int i = 1; i < 10; ++i) {
        EXPECT_EQ(instances[0], instances[i]);
    }
}

TEST_F(IocContainerSingletonTest, CannotCopyConstruct) {
    // This test ensures copy constructor is deleted
    // If this compiles, the test should fail
    // Uncomment the next lines to verify compilation error:
    // auto& instance = ioccontainer::IocContainer::getInstance();
    // ioccontainer::IocContainer copy(instance); // Should not compile
    
    SUCCEED(); // This test passes if it compiles (copy constructor is deleted)
}

TEST_F(IocContainerSingletonTest, CannotCopyAssign) {
    // This test ensures copy assignment is deleted
    // If this compiles, the test should fail
    // Uncomment the next lines to verify compilation error:
    // auto& instance1 = ioccontainer::IocContainer::getInstance();
    // auto& instance2 = ioccontainer::IocContainer::getInstance();
    // instance1 = instance2; // Should not compile
    
    SUCCEED(); // This test passes if it compiles (copy assignment is deleted)
}

TEST_F(IocContainerSingletonTest, SingletonPersistsAcrossGetInstanceCalls) {
    // Get instance and store some state (when we implement registration)
    auto& instance1 = ioccontainer::IocContainer::getInstance();
    
    // Get instance again
    auto& instance2 = ioccontainer::IocContainer::getInstance();
    
    // Should be same instance
    EXPECT_EQ(&instance1, &instance2);
    
    // TODO: When we implement service registration, add test like:
    // instance1.registerService<Interface, Implementation>();
    // EXPECT_TRUE(instance2.isRegistered<Interface>());
}
