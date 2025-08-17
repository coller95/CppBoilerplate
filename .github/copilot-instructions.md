# AI Coding Instructions - CppBoilerplate

## Core Architecture: Metaprogramming Auto-Registration

### **CRITICAL: ApiRouter Auto-Registration Architecture**

ApiRouter is the cornerstone of this project's **plugin discovery system** with automatic endpoint registration. **NO BYPASS POSSIBLE** - ALL HTTP requests must flow through the router:

```cpp
// MANDATORY: All HTTP traffic routes through ApiRouter
_webServer->setGlobalRequestHandler([&apiRouter](const auto& request, auto& response) {
    apiRouter.handleRequest(request.uri, request.method, request.body, responseBody, statusCode);
});
```

**Critical Benefits:**
- **Complete endpoint discoverability** - every endpoint visible via `/api/endpoints`
- **Centralized request logging** - all HTTP traffic logged uniformly
- **Consistent error handling** - unified 404/500 response format
- **Automatic API documentation** - endpoint registry enables docs generation

### Metaprogramming Patterns (85-90% Boilerplate Reduction)

#### Endpoints (CRTP Pattern)
```cpp
// OLD: Manual Registration (15+ lines)
namespace {
    struct EndpointUserRegistration {
        EndpointUserRegistration() {
            apirouter::ApiRouter::registerModuleFactoryGlobal([]() {
                return std::make_unique<EndpointUser>();
            });
        }
    };
    static EndpointUserRegistration _registration;
}

// NEW: Metaprogramming (1 line)
namespace { static apirouter::AutoRegister<EndpointUser> _autoRegister; }
```

**CRTP Endpoint Class:**
```cpp
class EndpointUser : public apirouter::AutoRegisterEndpoint<EndpointUser> {
public:
    void registerAvailableMethods(apirouter::IEndpointRegistrar& registrar, const std::string& basePath) override {
        registerMethod<&EndpointUser::handleGet>(registrar, "/user", "GET");
        registerMethod<&EndpointUser::handlePost>(registrar, "/user", "POST");
    }
protected:
    void handleGet(std::string_view path, std::string_view method, 
                   const std::string& requestBody, std::string& responseBody, int& statusCode);
    void handlePost(std::string_view path, std::string_view method, 
                    const std::string& requestBody, std::string& responseBody, int& statusCode);
};
```

#### Services (Template IoC Pattern)
```cpp
// OLD: Manual Registration (13+ lines)
namespace {
    struct ServiceUserRegistration {
        ServiceUserRegistration() {
            auto instance = std::make_shared<ServiceUser>();
            ioccontainer::IIocContainer::registerGlobal<IServiceUser>(instance);
            ioccontainer::IIocContainer::registerGlobal<ServiceUser>(instance);
        }
    };
    static ServiceUserRegistration _registration;
}

// NEW: Metaprogramming (1 line)
namespace { static ioccontainer::AutoRegister<ServiceUser, IServiceUser> _autoRegister; }
```

**Service Implementation:**
```cpp
class ServiceUser : public IServiceUser {
public:
    std::string processUserData() const override;  // Domain-specific naming
};
```

## TDD Workflow (MANDATORY)

**Rule**: Write failing test FIRST, then implement. Non-negotiable.

```bash
# 1. Write failing test in tests/ModuleTest/cases/
# 2. Build and verify RED
make test-run-ModuleTest
# 3. Implement minimal code
# 4. Build and verify GREEN  
# 5. Refactor and repeat
```

**Critical TDD Requirements:**
- **ONE CLASS PER FILE** - Mandatory for context efficiency
- **Test isolation** - Zero external module dependencies in unit tests
- **Mock business logic** - ApiRouter, Services, Domain logic
- **Isolate infrastructure** - WebServer, Logger, Database wrappers

## Module Generation (Differentiated Architecture)

### Progressive Module Enhancement
```bash
# Default: Simple concrete class (minimal overhead)
./scripts/create_module.sh create ConfigManager
# → Creates: ConfigManager.h, ConfigManager.cpp (no interface, no PIMPL)

# Add interface when polymorphism needed
./scripts/create_module.sh create Logger --interface  
# → Creates: ILogger.h, Logger.h, Logger.cpp, MockLogger.h

# Add PIMPL when hiding complex dependencies
./scripts/create_module.sh create WebServer --interface --pimpl
# → Creates: IWebServer.h, WebServer.h (with PIMPL), WebServer.cpp (with Impl class)
```

**When to use flags:**
- `--interface`: Multiple implementations, dependency injection, unit testing with mocks
- `--pimpl`: Hide complex dependencies (Mongoose, OpenSSL), reduce compilation time

### Metaprogramming Service Generation
```bash
./scripts/create_service.sh create ServiceUser --interface # Interface + Implementation + Mock
./scripts/create_service.sh create ServiceUser             # Simple service (concrete only)
```

### Metaprogramming Endpoint Generation
```bash
./scripts/create_endpoint.sh create EndpointUser
# → Creates: EndpointUser with CRTP auto-registration + clean handlers
```

## Testing Strategy

**Rule**: Mock business logic, isolate infrastructure.

```cpp
// ✅ MOCK: Business logic (ApiRouter, Services, Domain)
MockEndpoint mockEndpoint;
EXPECT_CALL(mockEndpoint, handleRequest(_)).Times(1);

// ✅ ISOLATE: Infrastructure (WebServer, Logger, Database)
WebServer webServer("127.0.0.1", 8080);
EXPECT_NO_THROW(webServer.start());
```

**Test Isolation Requirements:**
- `DEPENDENCIES =` (empty in test Makefiles for unit tests)
- No `#include` of other project modules in unit tests
- Use `IServiceA`, `IServiceB` (generic interfaces for testing)
- Test mechanics, not business logic
- One class per file enables surgical context targeting

```
APPLICATION LAYER    ← main.cpp orchestration
    ↓
BUSINESS LOGIC       ← ApiRouter + Endpoints (USE MOCKING)
    ↓
INFRASTRUCTURE       ← WebServer + Logger (USE ISOLATION)
```

## Module Architecture Standards

### Modular Monolithic Architecture
- All features in single codebase/binary, organized as self-contained modules
- Each module: own directory under `src/`, `include/`, `tests/`
- Modules communicate via well-defined interfaces and dependency injection
- High cohesion, low coupling, easier maintainability vs microservices

### Directory Structure Requirements
```
include/ModuleName/ModuleName.h
src/ModuleName/ModuleName.cpp
tests/ModuleNameTest/
    cases/               # Test cases subfolder
    Makefile
```

**Endpoint Module Structure:**
- Each endpoint is fully independent module (not under `ApiModule`)
- Example: `EndpointHello` → `include/EndpointHello/`, `src/EndpointHello/`, `tests/EndpointHelloTest/`
- Maximizes decoupling and modularity

## Build System Standards

**Makefiles Requirements:**
- Use `g++` for `.cpp`, `gcc` for `.c`
- Objects in `obj/`, binaries in `bin/`
- **Real tabs in recipe lines** (not spaces) - Required by GNU Make
- Pattern: `$(OBJDIR)/%.o: %.cpp`
- Test organization: Test cases in `cases/` subfolder

**WebServer Backend Selection:**
- Always use `enum class` for backend selection (not strings/integers)
- Example: `WebServer(ip, port, WebServer::Backend::Mongoose)`
- Type safety, discoverability, prevents stringly-typed API errors

## Coding Standards (Microsoft C++)

**Naming Conventions:**
- **PascalCase**: class names, structs, enums, namespaces
- **camelCase**: variables, function parameters, local functions
- **ALL_CAPS**: macros, compile-time constants
- **One file per class**: ModuleName.h, ModuleName.cpp

**Code Formatting:**
- Indent with tabs (4 spaces each)
- Opening braces on same line: `class Foo {`
- Use explicit types (avoid `auto` except lambdas/iterators)
- Prefer smart pointers over raw pointers
- Use `const` wherever possible

**Namespace Usage:**
- Always define namespace matching module name (lowercase)
- No `using namespace` in headers
- Prefer explicit qualification in implementation files

## Architecture Guidelines

### PIMPL Pattern (Generated Code Only)
- Use PIMPL in generated modules to hide implementation dependencies
- Keeps headers clean for script generation tooling
- Avoids exposing backend-specific includes

### Avoid Redundant Abstraction
- Don't create multiple interface layers that just forward calls
- **Avoid**: `Interface → Wrapper → Backend Interface → Concrete Backend` (4+ layers)
- **Better**: `Interface → Implementation (PIMPL) → Concrete Backend` (3 layers max)
- Each layer should add meaningful abstraction, not just delegation

### Performance Considerations
- PIMPL justified for tooling compatibility and dependency hiding
- Additional interfaces not justified if they only forward calls
- Each abstraction layer adds virtual call overhead
- Review: If layer only delegates without transformation, consider removing

## Common Pitfalls

**❌ DON'T:**
- Create WebServer endpoints that bypass ApiRouter
- Use isolation testing for ApiRouter (it's business logic)
- Assume `initialize()` processes factories registered after first call
- Use spaces instead of tabs in Makefile recipe lines
- Create multiple abstraction layers that only forward calls
- Use string/integer parameters for backend selection

**✅ DO:**
- Always use metaprogramming auto-registration patterns
- Test ApiRouter with mocked dependencies  
- Handle exceptions gracefully in endpoint handlers
- Use meaningful HTTP status codes
- Follow TDD: write failing test first, then implement
- Use one class per file for surgical context targeting
- Use enum class for type-safe backend selection

## Key Commands

```bash
# Generation (with metaprogramming patterns)
./scripts/create_module.sh create ModuleName [--interface] [--pimpl]
./scripts/create_service.sh create ServiceName [--interface]
./scripts/create_endpoint.sh create EndpointName

# Build and test
make debug && make test-run-ModuleTest
make release && make test
make clean

# Debug and analysis
make debug-config      # In test directories
make test-help         # Show all test targets
./scripts/generate_class_chart.sh  # UML diagrams

# TDD workflow verification
make test-run-ModuleName  # Should fail (RED)
# Implement minimal code
make test-run-ModuleName  # Should pass (GREEN)
```

## Critical Rules Summary

1. **TDD mandatory** - Write failing test first, always
2. **One class per file** - Mandatory for context efficiency  
3. **Metaprogramming auto-registration** - Use for endpoints/services (85-90% less boilerplate)
4. **ApiRouter centralization** - All HTTP traffic must flow through router
5. **Test isolation** - Mock business logic, isolate infrastructure
6. **Real tabs in Makefiles** - Required by GNU Make
7. **Progressive enhancement** - Start simple, add complexity only when needed
8. **Type-safe backend selection** - Use enum class, not strings

---

**Benefits**: Metaprogramming achieves 85-90% boilerplate reduction, type-safe compile-time registration, consistent patterns across endpoints and services, while maintaining architectural quality and testability.