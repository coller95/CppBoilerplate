# CLAUDE.md

Claude Code guidance for this C++ codebase.

## Commands

### Build/Test
```bash
./scripts/test.sh                 # Minimal mode: TEST=OK BUILD=OK RUN=OK TIME=42s
./scripts/test.sh human          # Human mode: emojis/colors  
./scripts/test.sh debug          # Debug mode: full output
./scripts/debug.sh               # Quick dev: test + build + run debug

make debug                       # Build debug
make release                     # Build release  
make test                        # Run all tests
make test-run-<ModuleName>       # Run specific module tests
make clean                       # Clean build artifacts
```

### Modes
- **Default**: LLM-optimized (90% token reduction)
- **human**: Human-friendly output
- **debug**: Full verbose output
- Use `VERBOSE=minimal|human|debug` for individual commands

### Generation
```bash
# Modules (progressive complexity)
./scripts/create_module.sh create ModuleName              # Simple class
./scripts/create_module.sh create ModuleName --interface  # + Interface
./scripts/create_module.sh create ModuleName --interface --pimpl # + PIMPL

# Services (always with interface)  
./scripts/create_service.sh create ServiceName            # Create simple service (no interface)"
./scripts/create_service.sh create ServiceName --simple   # Create simple service (explicit)"
./scripts/create_service.sh create ServiceName --interface  # Create service with interface and mock"

# Endpoints (metaprogramming auto-registration)
./scripts/create_endpoint.sh create EndpointName          # CRTP auto-registration + clean handlers

# Remove
./scripts/create_module.sh remove ModuleName
./scripts/create_service.sh remove ServiceName  
./scripts/create_endpoint.sh remove EndpointName
```

### LLM Tools  
```bash
# Analysis (90%+ token reduction)
./scripts/lexer.sh analyze src/                   # Structured JSON summary
./scripts/llm_analysis.sh analyze file.json type # Standardized analysis
./scripts/refactor.sh extract-class file Class   # Safe refactoring
```

## ⚠️ TDD MANDATORY

**STRICT RULES**:
- Write failing test FIRST, then implement
- NO code without tests
- ONE CLASS PER FILE 
- RED-GREEN-REFACTOR cycle

```bash
# TDD Workflow
make test-run-ModuleName  # RED (should fail)
# Write minimal code
make test-run-ModuleName  # GREEN (should pass) 
# Refactor + test again
```

## ⚠️ TEST ISOLATION MANDATORY

**RULE**: Zero external module dependencies. Test only what you're testing.

**❌ WRONG**: 
```cpp
#include <Logger/Logger.h>  // External dependency!
auto logger = std::make_shared<logger::Logger>(...);
```

**✅ CORRECT**:
```cpp
// Generic test services only
class IServiceA { virtual int getValue() = 0; };
class ServiceAImpl : public IServiceA { /*...*/ };
```

**Requirements**:
- `DEPENDENCIES =` (empty in Makefile)
- No `#include` of other project modules
- Use `IServiceA`, `IServiceB` (generic interfaces)
- Test mechanics, not business logic

## Testing Strategy

**RULE**: Mock business logic, isolate infrastructure.

**MOCK** (ApiRouter, Services, Domain logic):
```cpp
MockEndpoint mockEndpoint;
EXPECT_CALL(mockEndpoint, handleRequest(_)).Times(1);
```

**ISOLATE** (WebServer, Logger, DB wrappers):
```cpp  
WebServer webServer("127.0.0.1", 8080);
EXPECT_NO_THROW(webServer.start());
```

**Guidelines**:
- Infrastructure: Test interface contracts, not external libraries
- Business Logic: Mock dependencies, test decision logic
- Integration tests separate from unit tests

## Architecture

**Modular monolithic architecture** with:

**Core Components**:
- `main.cpp`: IoC + WebServer + ApiRouter orchestration
- `IocContainer`: Dependency injection 
- `WebServer`: HTTP abstraction (self-contained backends)
- `ApiRouter`: Endpoint management + auto-registration
- `Logger`: Local/remote logging

**Critical Rules**:
- ONE CLASS PER FILE (mandatory)
- No leaky abstractions (no backend-specific methods like `poll()`)
- All HTTP requests MUST go through ApiRouter (no bypass)
- Auto-registration pattern for all modules
- Self-contained components handle own threading

**Metaprogramming Endpoints** (NEW):
- Use `AutoRegisterEndpoint<T>` CRTP base class
- Automatic path inference: `EndpointUser` → `/user`
- Clean method naming: `handleGet()` vs `handleGetUser()`
- Type-safe registration: `registerMethod<&Class::method>()`
- One-line registration: `AutoRegister<Class> _autoRegister;`
- 85% less boilerplate than manual registration

**Layer Testing**:
- Infrastructure (WebServer, Logger): ISOLATE
- Business Logic (ApiRouter, Services): MOCK  
- Integration: Separate test modules

**LLM Optimization**:
- Minimal mode default (90-95% token reduction)
- One class per file enables surgical context targeting
- Structured JSON analysis preferred over raw C++ reading

## Metaprogramming Endpoints

**NEW PATTERN**: Auto-registration using CRTP (Curiously Recurring Template Pattern)

### Before (Manual Registration)
```cpp
// Header: Verbose interface
class EndpointUser : public apirouter::IApiModule {
public:
    void registerEndpoints(apirouter::IEndpointRegistrar& registrar) override;
protected:
    void handleGetUser(std::string_view path, std::string_view method, 
                       const std::string& requestBody, std::string& responseBody, int& statusCode);
};

// Implementation: Manual lambda wrapping (15+ lines)
void EndpointUser::registerEndpoints(apirouter::IEndpointRegistrar& registrar) {
    registrar.registerHttpHandler("/user", "GET",
        [this](std::string_view path, std::string_view method, const std::string& requestBody, 
                std::string& responseBody, int& statusCode) {
            handleGetUser(path, method, requestBody, responseBody, statusCode);
        });
}

// Manual registration struct (8+ lines)
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
```

### After (Metaprogramming Auto-Registration)
```cpp
// Header: Clean CRTP inheritance
class EndpointUser : public apirouter::AutoRegisterEndpoint<EndpointUser> {
public:
    void registerAvailableMethods(apirouter::IEndpointRegistrar& registrar, const std::string& basePath) override;
protected:
    void handleGet(std::string_view path, std::string_view method, 
                   const std::string& requestBody, std::string& responseBody, int& statusCode);
};

// Implementation: Type-safe registration (3 lines)
void EndpointUser::registerAvailableMethods(apirouter::IEndpointRegistrar& registrar, const std::string& basePath) {
    registerMethod<&EndpointUser::handleGet>(registrar, "/user", "GET");
    // registerMethod<&EndpointUser::handlePost>(registrar, "/user", "POST");
}

// One-line registration!
namespace {
    static apirouter::AutoRegister<EndpointUser> _autoRegister;
}
```

### Benefits
- **85% less boilerplate** - No manual lambda wrapping
- **Type safety** - Compile-time method signature verification  
- **Clean naming** - `handleGet()` vs `handleGetUser()`
- **Easy expansion** - Add new methods with one line
- **CRTP pattern** - Zero runtime overhead
- **KISS/YAGNI compliant** - Simple and minimal

### Usage
```bash
# Generate metaprogramming endpoint
./scripts/create_endpoint.sh create EndpointUser

# Implementation steps:
# 1. Implement handleGet() method
# 2. Add registerMethod<&Class::handlePost>() calls as needed  
# 3. Test with: make test-run-EndpointUserTest
```

## Memory Management

**Clear `/clear` when**:
- Major context shift (different modules)
- Architecture changes (core interfaces)
- Context pollution (references non-existent files)
- Long sessions (30+ tool calls, 1-2+ hours)

**Keep memory when**:
- Same module/feature incremental work
- TDD cycle within same class
- Context remains accurate

## Essential Rules

⚠️ **TDD MANDATORY**: Write failing test FIRST, then implement
⚠️ **LLM OPTIMIZATION**: Always use minimal mode (90-95% token reduction)  
⚠️ **ONE CLASS PER FILE**: Mandatory for context efficiency
⚠️ **TEST ISOLATION**: Zero external module dependencies
