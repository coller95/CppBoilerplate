#include <gtest/gtest.h>
#include <IoCContainer/IoCContainer.h>
#include <ServiceB/ServiceB.h>

TEST(ServiceBRegistrationTest, ServiceBIsRegisteredInIoC) {
    IoCContainer container;
    container.importGlobals();
    EXPECT_NO_THROW({
        auto ptr = container.resolve<serviceb::ServiceB>();
        EXPECT_NE(ptr, nullptr);
    });
}
