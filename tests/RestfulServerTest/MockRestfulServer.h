#pragma once
#include "RestfulServer/RestfulServer.h"
#include <gmock/gmock.h>

class MockRestfulServer : public RestfulServer {
public:
    MockRestfulServer() : RestfulServer(8080) {}
    MOCK_METHOD(void, registerHandler, (const std::string& path, HandlerFunc handler), (override));
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, isRunning, (), (const, override));
};
