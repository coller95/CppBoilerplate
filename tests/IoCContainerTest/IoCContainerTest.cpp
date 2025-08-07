#include <gtest/gtest.h>
#include <memory>
#include "IoCContainer/IoCContainer.h"

class IFoo {
public:
    virtual ~IFoo() = default;
    virtual int getValue() const = 0;
};

class Foo : public IFoo {
public:
    Foo(int v) : _value(v) {}
    int getValue() const override { return _value; }
private:
    int _value;
};

TEST(IoCContainerTest, RegisterAndResolve) {
    IoCContainer container;
    container.registerType<IFoo>([]() {
        return std::make_shared<Foo>(42);
    });
    auto foo = container.resolve<IFoo>();
    ASSERT_EQ(foo->getValue(), 42);
}

TEST(IoCContainerTest, ThrowsOnUnregisteredType) {
    IoCContainer container;
    EXPECT_THROW(container.resolve<IFoo>(), std::runtime_error);
}
