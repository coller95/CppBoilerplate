#include <gtest/gtest.h>
#include <IocContainer/IIocContainer.h>
#include <Logger/Logger.h>
#include <Logger/ILogger.h>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <random>

namespace {

class IocConcurrencyTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean slate for each test
        auto& container = ioccontainer::IIocContainer::getInstance();
        container.clear();
    }
};

// Mock service that can track concurrent usage
class ConcurrentTrackingService {
private:
    std::atomic<int> _activeUsers{0};
    std::atomic<int> _totalRequests{0};
    std::atomic<int> _maxConcurrentUsers{0};
    
public:
    void processRequest() {
        int current = _activeUsers.fetch_add(1) + 1;
        _totalRequests.fetch_add(1);
        
        // Update max concurrent users
        int currentMax = _maxConcurrentUsers.load();
        while (current > currentMax && !_maxConcurrentUsers.compare_exchange_weak(currentMax, current)) {
            // Retry if another thread updated max
        }
        
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        
        _activeUsers.fetch_sub(1);
    }
    
    int getTotalRequests() const { return _totalRequests.load(); }
    int getMaxConcurrentUsers() const { return _maxConcurrentUsers.load(); }
};

} // anonymous namespace

// Test REALISTIC concurrency: HTTP requests resolving services
TEST_F(IocConcurrencyTest, ServiceResolutionUnderHTTPLoad) {
    // Setup: Register services like a real web server
    auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    logger->setLocalDisplay(true);
    ioccontainer::IIocContainer::registerGlobal<logger::ILogger>(logger);
    
    auto trackingService = std::make_shared<ConcurrentTrackingService>();
    ioccontainer::IIocContainer::registerGlobal<ConcurrentTrackingService>(trackingService);
    
    // Simulate: 100 concurrent HTTP requests
    const int numRequests = 100;
    std::vector<std::thread> requests;
    std::atomic<int> successCount{0};
    std::atomic<int> errorCount{0};
    
    for (int i = 0; i < numRequests; ++i) {
        requests.emplace_back([&]() {
            try {
                // Each HTTP request resolves services it needs
                auto requestLogger = ioccontainer::IIocContainer::resolveGlobal<logger::ILogger>();
                auto requestService = ioccontainer::IIocContainer::resolveGlobal<ConcurrentTrackingService>();
                
                if (requestLogger != nullptr && requestService != nullptr) {
                    // Use the services (simulate HTTP request processing)
                    requestLogger->logInfo("Processing HTTP request");
                    requestService->processRequest();
                    successCount++;
                } else {
                    errorCount++;
                }
            } catch (const std::exception& e) {
                errorCount++;
            }
        });
    }
    
    // Wait for all requests to complete
    for (auto& req : requests) {
        req.join();
    }
    
    // Verify: All requests succeeded, no concurrency failures
    EXPECT_EQ(successCount.load(), numRequests);
    EXPECT_EQ(errorCount.load(), 0);
    EXPECT_EQ(trackingService->getTotalRequests(), numRequests);
    EXPECT_GT(trackingService->getMaxConcurrentUsers(), 1); // Actually ran concurrently
}

// Test service resolution and registration happening concurrently
TEST_F(IocConcurrencyTest, ConcurrentRegistrationAndResolution) {
    const int numResolvers = 50;
    const int numRegistrars = 10;
    std::atomic<int> resolveSuccesses{0};
    std::atomic<int> resolveFailures{0};
    std::atomic<int> registrationsDone{0};
    
    std::vector<std::thread> threads;
    
    // Threads that try to resolve services
    for (int i = 0; i < numResolvers; ++i) {
        threads.emplace_back([&]() {
            // Add some randomness to thread scheduling
            std::this_thread::sleep_for(std::chrono::microseconds(rand() % 1000));
            
            try {
                auto logger = ioccontainer::IIocContainer::resolveGlobal<logger::ILogger>();
                if (logger != nullptr) {
                    logger->logInfo("Resolved during concurrent access");
                    resolveSuccesses++;
                } else {
                    resolveFailures++;
                }
            } catch (const ioccontainer::ServiceNotRegisteredException&) {
                resolveFailures++;
            } catch (...) {
                resolveFailures++;
            }
        });
    }
    
    // Threads that register services
    for (int i = 0; i < numRegistrars; ++i) {
        threads.emplace_back([&, i]() {
            // Add some randomness to thread scheduling
            std::this_thread::sleep_for(std::chrono::microseconds(rand() % 500));
            
            try {
                auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000 + i);
                logger->setLocalDisplay(true);
                ioccontainer::IIocContainer::registerGlobal<logger::ILogger>(logger);
                registrationsDone++;
            } catch (...) {
                // Registration failed
            }
        });
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    // Verify: No crashes, deadlocks, or corruption
    // Note: Some resolutions may fail (expected if they ran before registration)
    EXPECT_GT(registrationsDone.load(), 0); // At least some registrations succeeded
    EXPECT_EQ(resolveSuccesses.load() + resolveFailures.load(), numResolvers); // All resolvers completed
    
    // Final state should be consistent
    auto& container = ioccontainer::IIocContainer::getInstance();
    EXPECT_EQ(container.getRegisteredCount(), 1); // Only one logger type (overwrites)
}

// Test singleton behavior under thread stress
TEST_F(IocConcurrencyTest, SingletonStressTest) {
    const int numThreads = 100;
    std::vector<std::thread> threads;
    std::vector<ioccontainer::IIocContainer*> instances(numThreads);
    
    // Multiple threads all trying to get singleton instance
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&instances, i]() {
            // Get instance and immediately use it
            auto& container = ioccontainer::IIocContainer::getInstance();
            instances[i] = &container;
            
            // Immediately use the container (stress the singleton)
            EXPECT_EQ(container.getRegisteredCount(), 0);
        });
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    // Verify: All threads got the same singleton instance
    for (int i = 1; i < numThreads; ++i) {
        EXPECT_EQ(instances[0], instances[i]);
    }
}

// Test real-world scenario: web server startup + concurrent requests
TEST_F(IocConcurrencyTest, WebServerStartupWithConcurrentRequests) {
    std::atomic<bool> startupComplete{false};
    std::atomic<int> requestsProcessed{0};
    std::atomic<int> requestsFailed{0};
    const int numRequests = 50;
    
    std::vector<std::thread> threads;
    
    // Simulate web server startup thread
    threads.emplace_back([&]() {
        auto& container = ioccontainer::IIocContainer::getInstance();
        
        // Simulate gradual service registration (like real startup)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
        logger->setLocalDisplay(true);
        container.registerInstance<logger::ILogger>(logger);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto trackingService = std::make_shared<ConcurrentTrackingService>();
        container.registerInstance<ConcurrentTrackingService>(trackingService);
        
        startupComplete = true;
    });
    
    // Simulate early HTTP requests (may arrive during startup)
    for (int i = 0; i < numRequests; ++i) {
        threads.emplace_back([&]() {
            // Some requests come early, some after startup
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 50));
            
            try {
                auto logger = ioccontainer::IIocContainer::resolveGlobal<logger::ILogger>();
                auto service = ioccontainer::IIocContainer::resolveGlobal<ConcurrentTrackingService>();
                
                if (logger && service) {
                    logger->logInfo("Request processed successfully");
                    service->processRequest();
                    requestsProcessed++;
                } else {
                    requestsFailed++;
                }
            } catch (...) {
                requestsFailed++;
            }
        });
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    // Verify: Startup completed and most requests eventually succeeded
    EXPECT_TRUE(startupComplete.load());
    EXPECT_GT(requestsProcessed.load(), 0); // At least some requests worked
    EXPECT_EQ(requestsProcessed.load() + requestsFailed.load(), numRequests); // All accounted for
}

// Test memory safety under concurrent access
TEST_F(IocConcurrencyTest, MemorySafetyUnderConcurrentAccess) {
    const int numThreads = 20;
    const int operationsPerThread = 100;
    std::atomic<int> totalOperations{0};
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, i]() {
            for (int op = 0; op < operationsPerThread; ++op) {
                try {
                    // Mix of operations to stress memory management
                    if (op % 3 == 0) {
                        // Register service
                        auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000 + i);
                        logger->setLocalDisplay(true);
                        ioccontainer::IIocContainer::registerGlobal<logger::ILogger>(logger);
                    } else if (op % 3 == 1) {
                        // Resolve service  
                        auto logger = ioccontainer::IIocContainer::resolveGlobal<logger::ILogger>();
                        if (logger) {
                            logger->logInfo("Memory safety test");
                        }
                    } else {
                        // Query container state
                        auto& container = ioccontainer::IIocContainer::getInstance();
                        container.getRegisteredCount();
                    }
                    totalOperations++;
                } catch (...) {
                    // Some operations may fail, but shouldn't crash
                }
            }
        });
    }
    
    // Wait for all operations
    for (auto& t : threads) {
        t.join();
    }
    
    // Verify: No crashes, memory corruption, or resource leaks
    // Just completing without crashing is the main test
    EXPECT_GT(totalOperations.load(), 0);
    
    // Container should still be functional
    auto& container = ioccontainer::IIocContainer::getInstance();
    EXPECT_NO_THROW(container.getRegisteredCount());
}