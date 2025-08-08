#include <gtest/gtest.h>
#include <ServiceB/ServiceB.h>

TEST(ServiceBTest, DoSomethingServiceBReturnsExpected) {
    serviceb::ServiceB service;
    EXPECT_EQ(service.doSomethingServiceB(), "ServiceB result");
}
