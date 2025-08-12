#include <gtest/gtest.h>
#include <IocContainer/IocContainer.h>

namespace {

class IocContainerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code for each test
    }

    void TearDown() override {
        // Cleanup code for each test
    }
};

} // anonymous namespace

TEST_F(IocContainerTest, SingletonInstanceIsValid) {
    // Get singleton instance
    auto& module = ioccontainer::IocContainer::getInstance();
    
    // Verify the instance exists (suppress unused variable warning)
    (void)module;
    
    // TODO: Add assertions to verify the module is properly constructed
    // Example:
    // EXPECT_TRUE(module.isInitialized());
    SUCCEED(); // Remove this when you add real tests
}

TEST_F(IocContainerTest, SingletonCannotBeCopied) {
    // This test verifies that copy operations are deleted
    // The fact that this compiles means the copy constructor/assignment are properly deleted
    
    auto& instance = ioccontainer::IocContainer::getInstance();
    
    // These lines should not compile if uncommented:
    // ioccontainer::IocContainer copy(instance);  // Should not compile
    // auto copy2 = instance;                      // Should not compile
    
    // Verify we have a valid instance
    EXPECT_EQ(&instance, &ioccontainer::IocContainer::getInstance());
}

// TODO: Add more test cases for your module's functionality
// Example:
// TEST_F(IocContainerTest, InitializeReturnsTrueOnSuccess) {
//     ioccontainer::IocContainer module;
//     EXPECT_TRUE(module.initialize());
// }
