#include <gtest/gtest.h>
#include <IoCContainer/IoCContainer.h>
#include <ServiceA/ServiceA.h>

TEST(ServiceARegistrationTest, ServiceAIsRegisteredInIoC) {
    IoCContainer container;
    servicea::ServiceA::registerWith(container);
    EXPECT_NO_THROW({
        auto ptr = container.resolve<servicea::ServiceA>();
        EXPECT_NE(ptr, nullptr);
    });
}
