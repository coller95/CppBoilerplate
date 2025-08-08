#include <gtest/gtest.h>
#include <ServiceA/ServiceA.h>

TEST(ServiceATest, DoSomethingServiceAReturnsExpected) {
	servicea::ServiceA service;
	EXPECT_EQ(service.doSomethingServiceA(), "ServiceA result");
}
