#include <gtest/gtest.h>
#include <ServiceB/ServiceB.h>

TEST(ServiceBTest, DoSomethingBReturnsExpected) {
    serviceb::ServiceB service;
    EXPECT_EQ(service.doSomethingB(), "ServiceB result");
}
