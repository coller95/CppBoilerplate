#include <gtest/gtest.h>
#include <IoCContainer/IoCContainer.h>
#include <ServiceA/ServiceA.h>
#include <ServiceB/ServiceB.h>

// Add more includes for other services as needed

TEST(IoCContainerRegistrationTest, AllExpectedServicesAreRegistered)
{
    IoCContainer container;
    // Register services as in your Application constructor or rely on static registration
    container.registerType<servicea::ServiceA>([]() { return std::make_shared<servicea::ServiceA>(); });
    container.registerType<serviceb::ServiceB>([]() { return std::make_shared<serviceb::ServiceB>(); });
    // ...add more as needed

    // Check that resolving does not throw and returns a valid pointer
    EXPECT_NE(container.resolve<servicea::ServiceA>(), nullptr);
    EXPECT_NE(container.resolve<serviceb::ServiceB>(), nullptr);
    // ...add more as needed
}
