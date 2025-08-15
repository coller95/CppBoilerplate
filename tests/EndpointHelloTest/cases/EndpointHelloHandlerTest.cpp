#include <gtest/gtest.h>
#include <EndpointHello/EndpointHello.h>
#include <string>

class EndpointHelloHandlerTest : public ::testing::Test, protected endpointhello::EndpointHello {
protected:
    // Test class inherits from EndpointHello to access protected handler methods
};

TEST_F(EndpointHelloHandlerTest, HandleGetHelloReturnsSuccessResponse) {
    std::string responseBody;
    int statusCode = 0;
    
    // Test the extracted handler method directly
    handleGetHello("/hello", "GET", "", responseBody, statusCode);
    
    // Verify response
    EXPECT_EQ(statusCode, 200);
    EXPECT_FALSE(responseBody.empty());
    EXPECT_NE(responseBody.find("EndpointHello"), std::string::npos);
}

TEST_F(EndpointHelloHandlerTest, HandleGetHelloAcceptsParameters) {
    std::string responseBody;
    int statusCode = 0;
    std::string requestBody = R"({"test": "data"})";
    
    // Test with request body
    handleGetHello("/hello/123", "GET", requestBody, responseBody, statusCode);
    
    // Should still return success (parameters are ignored in this simple implementation)
    EXPECT_EQ(statusCode, 200);
    EXPECT_FALSE(responseBody.empty());
}

// TODO: Add tests for additional handler methods when implemented
// TEST_F(EndpointHelloHandlerTest, HandlePostHelloCreatesResource) {
//     std::string responseBody;
//     int statusCode = 0;
//     std::string requestBody = R"({"name": "testHello"})";
//     
//     handlePostHello("/hello", "POST", requestBody, responseBody, statusCode);
//     
//     EXPECT_EQ(statusCode, 201);
//     EXPECT_FALSE(responseBody.empty());
// }
