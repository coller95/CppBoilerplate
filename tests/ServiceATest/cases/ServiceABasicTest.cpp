#include <gtest/gtest.h>
#include <ServiceA/ServiceA.h>

TEST(ServiceATest, DoSomethingAReturnsExpected) {
    servicea::ServiceA service;
    EXPECT_EQ(service.doSomethingA(), "ServiceA result");
}
