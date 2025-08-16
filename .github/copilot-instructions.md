# AI Coding Instructions for CppBoilerplate

This document provides comprehensive guidelines for AI-assisted coding in the CppBoilerplate project. It includes coding standards, architectural principles, build system requirements, and test-driven development (TDD) practices to ensure high-quality, maintainable code.

## Table of Contents

1. [Project Architecture](#project-architecture)
2. [Coding Standards](#coding-standards)
3. [Module Development](#module-development)
4. [Build System Standards](#build-system-standards)
5. [Test-Driven Development (TDD)](#test-driven-development-tdd)
6. [Development Workflow](#development-workflow)
7. [AI/Assistant Guidance](#aiassistant-guidance)

---

## Project Architecture

### Modular Monolithic Architecture

This project follows a **modular monolithic architecture**:

- All features and modules are part of a single codebase and binary, but are organized into clear, self-contained modules
- Each module has its own directory under `src/` and `include/`, with separate tests under `tests/`
- Modules communicate via well-defined interfaces and dependency injection, not global state
- Shared utilities and interfaces are placed in dedicated shared modules
- This approach enables high cohesion, low coupling, and easier maintainability while avoiding the complexity of microservices
- Each module should have its own test suite, with object files in `obj/` and test executables in `bin/`

### Dependency Management

- Design code for easy unit testing and minimal dependencies
- Favor dependency injection and clear interfaces for all modules to enable mocking and isolation in tests
- Avoid global state and side effects; always pass dependencies explicitly

### **CRITICAL: ApiRouter Auto-Registration Architecture**

**ApiRouter** is the cornerstone of this project's **plugin discovery system** with automatic endpoint registration. Understanding this architecture is essential for all HTTP endpoint development.

#### **Core Concept: Single Source of Truth**

ApiRouter enforces **NO BYPASS POSSIBLE** architecture where ALL HTTP requests must flow through the router:

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

#### **Two-Phase Initialization Pattern**

**Phase 1: Static Initialization (Automatic)**
```cpp
// In each endpoint .cpp file
namespace {
    struct EndpointHelloRegistration {
        EndpointHelloRegistration() {
            apirouter::ApiRouter::registerModuleFactoryGlobal([]() {
                return std::make_unique<EndpointHello>();
            });
        }
    };
    static EndpointHelloRegistration _registration;  // Auto-registers at startup
}
```

**Phase 2: Runtime Initialization (Explicit)**
```cpp
// In main.cpp
int main() {
    apirouter::ApiRouter::initializeGlobal();  // Creates modules, registers endpoints
    // System ready - all endpoints discoverable
}
```

#### **Architecture Layer: Business Logic**

```
APPLICATION LAYER    ← main.cpp orchestration
    ↓
BUSINESS LOGIC       ← ApiRouter + Endpoints (USE MOCKING)
    ↓
INFRASTRUCTURE       ← WebServer + Logger (USE ISOLATION)
```

**Testing Approach**: ApiRouter is business logic, so use **MOCKING strategy**:

```cpp
// ✅ CORRECT: Mock infrastructure dependencies
TEST(ApiRouterTest, RoutesToCorrectEndpoint) {
    MockWebServer mockWebServer;
    EXPECT_CALL(mockWebServer, setGlobalRequestHandler(_)).Times(1);
    
    ApiRouter router;
    router.configureWebServer(mockWebServer);
}
```

#### **Performance & Safety Features**

**Thread Safety:**
- Meyer's singleton pattern (static initialization safety)
- Mutex-protected endpoint registry
- Concurrent registration and routing support

**Exception Safety:**
- Module factory failures don't break system initialization
- Handler exceptions return proper 500 errors with details
- Input validation prevents crashes from malformed requests

**Performance Optimizations:**
- Standard mutex (not recursive) for better performance
- Copy-then-release locking strategy
- Null handler checks prevent crashes

#### **Development Workflow**

```bash
# 1. Create endpoint (auto-registration included)
./scripts/create_endpoint.sh create EndpointUser

# 2. Test endpoint with proper mocking
make test-run-EndpointUserTest

# 3. Verify registration worked
curl http://localhost:8080/api/endpoints
```

#### **Common Pitfalls**

**❌ DON'T:**
- Create WebServer endpoints that bypass ApiRouter
- Use isolation testing for ApiRouter (it's business logic)
- Assume `initialize()` processes factories registered after first call

**✅ DO:**
- Always use auto-registration pattern for endpoints
- Test ApiRouter with mocked dependencies
- Handle exceptions gracefully in endpoint handlers
- Use meaningful HTTP status codes

---

## Coding Standards

### Microsoft C++ Coding Style

- **Naming Conventions:**
  - Use PascalCase for class names, structs, enums, and namespaces
  - Use camelCase for variable names, function parameters, and local functions
  - Use ALL_CAPS for macros and compile-time constants
  - One file per class: each class should have its own header (.h) and source (.cpp) file, named after the class

- **Code Formatting:**
  - Indent with tabs, each tab 4 spaces
  - Place opening braces on the next line after the declaration
  - Group related declarations and functions together logically

- **Modern C++ Practices:**
  - Use explicit types (avoid `auto` except for lambdas and iterators)
  - Prefer smart pointers (`std::unique_ptr`, `std::shared_ptr`) over raw pointers
  - Use `const` wherever possible for variables, parameters, and member functions

- **Documentation:**
  - Add comments to public interfaces and complex or non-obvious logic
  - Clearly document the purpose of each test and mock

### Namespace Usage

- Always define a namespace for each feature or module
- The namespace should match the feature/module name and group all related classes and functions
- Do not use `using namespace` in header files
- In implementation files, prefer explicit namespace qualification for clarity and to avoid symbol conflicts

### WebServer Backend Selection

- When adding or updating the WebServer implementation, always use an `enum class` to select the backend (e.g., Mongoose, Other) instead of string or integer parameters
- The `WebServer` constructor must accept a `Backend` enum parameter, defaulting to the primary backend (e.g., Mongoose)
- All backend selection logic (including factories) must use this enum for clarity, type safety, and extensibility
- Example usage:
  ```cpp
  std::unique_ptr<WebServer> server = std::make_unique<WebServer>(ip, port, WebServer::Backend::Mongoose);
  ```
- Update all tests and documentation to use the enum-based selection pattern

**Rationale:** Using an enum for backend selection ensures type safety, discoverability, and prevents errors from stringly-typed APIs. It also makes adding new backends straightforward and consistent.

### Generated Module Architecture Guidelines

When working with generated modules or modifying generation scripts, follow these architectural principles:

#### PIMPL Pattern for Generated Code
- **Use PIMPL idiom** in generated modules to hide implementation dependencies from headers
- **Purpose**: Keeps headers clean for script generation tooling and avoids exposing backend-specific includes
- **Justification**: Generation scripts need stable public interfaces without implementation complexity

#### Avoid Redundant Abstraction Layers
- **Problem**: Don't create multiple interface layers that just forward calls without adding value
- **Example to Avoid**: `Interface → Wrapper → Backend Interface → Concrete Backend` (4+ layers)
- **Better Approach**: `Interface → Implementation (PIMPL) → Concrete Backend` (3 layers max)
- **Focus**: Each layer should add meaningful abstraction, not just delegation

#### Balance Tool Compatibility vs Performance
- **PIMPL is justified** for script generation and tooling compatibility
- **Additional interfaces are not justified** if they only forward calls to other interfaces
- **Performance consideration**: Each abstraction layer adds virtual call overhead
- **Guideline**: If a layer only delegates to another layer without transformation, consider removing it

#### Generation Script Flexibility
- **Keep generation scripts simple** - avoid creating unnecessary architectural complexity
- **Direct backend usage**: Consider if generated code can use concrete implementations directly
- **Interface segregation**: Only create interfaces that provide genuine abstraction value
- **Review regularly**: When modifying generation scripts, assess if the current architecture is optimal

#### Architecture Assessment Questions
When reviewing generated code architecture, ask:
1. Does each abstraction layer add unique value?
2. Can we eliminate forwarding-only layers?
3. Is PIMPL sufficient for dependency hiding?
4. Are we optimizing for tooling at the expense of runtime performance?
5. Can concrete implementations be used more directly?

**Rationale:** Generated code should balance tooling requirements (PIMPL for clean headers) with performance considerations (minimal abstraction layers). Each layer should justify its existence through genuine added value, not just code organization.

---

## Module Development

### Endpoint Module Structure

- Each endpoint must be a fully independent module, just like services
- Each endpoint should have its own folder under `include/`, `src/`, and `tests/`, not grouped under `ApiModule` or any other shared folder
- For example, for an endpoint named `EndpointHello`:
  - `include/EndpointHello/EndpointHello.h`
  - `src/EndpointHello/EndpointHello.cpp`
  - `tests/EndpointHelloTest/` (with its own test cases in `cases/`)
- This structure should be used for all endpoints (e.g., `EndpointA`, `EndpointB`, etc.) to maximize decoupling and modularity
- All test cases for endpoints must be placed in their respective test module folders, following the `cases/` subfolder convention

### Endpoint Generation Script

- Use the `scripts/create_endpoint.sh` script to generate new endpoint modules automatically
- Usage:
  - Create: `./scripts/create_endpoint.sh create EndpointName` (must start with "Endpoint" followed by a capitalized name)
  - Remove: `./scripts/create_endpoint.sh remove EndpointName` (removes an existing endpoint with confirmation)
- The script creates the complete modular structure with auto-registration cross-check tests
- Always test the generated endpoint with `make test-run-EndpointNameTest`

### Service Generation Script

- Use the `scripts/create_service.sh` script to generate new service modules automatically
- Usage:
  - Create: `./scripts/create_service.sh create ServiceName` (must start with "Service" followed by a capitalized name)
  - Remove: `./scripts/create_service.sh remove ServiceName` (removes an existing service with confirmation)
- The script creates the complete modular structure with IoC registration verification tests
- Always test the generated service with `make test-run-ServiceNameTest`

### **NEW: Differentiated Module Generation Architecture**

**CRITICAL**: This project uses a **differentiated generation approach** - each script type follows different complexity defaults based on actual usage patterns:

#### **🔧 create_module.sh - Progressive Enhancement**
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

#### **🔧 create_service.sh - Interface by Default**
```bash
# Always generates interface (needed for DI and testing)
./scripts/create_service.sh create ServiceUser
# → Creates: IServiceUser + ServiceUser + MockServiceUser + auto-registration
```

**Key improvements:**
- Simplified auto-registration (no redundant methods)
- Domain-specific method naming (`processUserData()` not `doSomethingServiceUser()`)
- Always includes interface for dependency injection and testing

#### **🔧 create_endpoint.sh - Testable Handler Methods**
```bash
# Generates testable handler methods (no interface needed)
./scripts/create_endpoint.sh create EndpointUser
# → Creates: EndpointUser with protected handleGetUser() method + handler tests
```

**Architecture benefits:**
- Handler logic extracted to testable protected methods
- Lambdas are thin wrappers calling handler methods  
- Direct unit testing of HTTP handler logic
- Each HTTP method gets its own handler method

### **🎯 Key Architectural Improvements**

**1. Minimal Overhead by Default:**
- Utility modules start as simple concrete classes
- No premature abstraction or unused patterns
- Add complexity only when requirements justify it

**2. Service-Oriented Design:**
- Services always get interfaces (required for DI/testing)
- Simplified auto-registration (no redundant methods)
- Domain-specific method naming

**3. Testable Endpoint Architecture:**
- HTTP handler logic extracted from lambdas
- Protected methods accessible to unit tests
- Clear separation: registration logic vs. business logic

**4. Progressive Enhancement:**
- Start simple: `create ConfigManager` → minimal class
- Add interface: `--interface` → enables polymorphism  
- Add PIMPL: `--pimpl` → hides implementation complexity
- Follow YAGNI: "You Aren't Gonna Need It" principle

**Rationale:** Different component types have different complexity needs by default. This differentiated approach eliminates over-engineering while providing the right abstractions when actually needed.

### Class Chart Generation Script

- Use the `scripts/generate_class_chart.sh` script to generate dynamic UML class diagrams automatically
- Usage: `./scripts/generate_class_chart.sh` (no arguments required)
- The script generates comprehensive class charts using Doxygen and Graphviz:
  - Automatically creates or updates `Doxyfile` configuration
  - Generates HTML documentation with embedded UML diagrams
  - Creates interactive class hierarchy charts showing relationships
  - Outputs SVG format diagrams for high-quality visualization
- Generated documentation is placed in `docs/` directory
- View results by opening `docs/index.html` in a browser
- Direct class diagram access via `docs/hierarchy.html`

**Key Features:**
- **Automatic dependency detection**: Checks for Doxygen and Graphviz installation
- **Project-specific configuration**: Automatically configures Doxyfile for CppBoilerplate structure
- **Interactive UML diagrams**: Click-able class relationships and inheritance hierarchies
- **Integration with documentation**: Embeds UML link directly in main documentation page
- **High-quality output**: SVG format ensures scalable, crisp diagrams

**Dependencies Required:**
```bash
# Install required tools (Ubuntu/Debian)
sudo apt-get install doxygen graphviz

# Then generate charts
./scripts/generate_class_chart.sh
```

**Rationale:** Visual class diagrams provide immediate architectural understanding, help identify design patterns, reveal coupling issues, and serve as living documentation that stays current with code changes. Essential for understanding complex class hierarchies and relationships in modular architectures.

### Test File Type Standardization

- For all test cases that require file input/output, use binary file types instead of `.txt` files
- This applies to any test that interacts with files, ensuring consistency and clarity in test data handling
- Avoid using `.txt` files for new or updated tests; prefer binary files (e.g., `.bin`) to standardize file-based testing across the project

**Rationale:** Using binary files for tests ensures that file handling logic is robust and not dependent on text encoding or formatting, and it provides a consistent approach for all contributors.

---

# Endpoint Modularization Convention

## Endpoint Module Structure

- Each endpoint must be a fully independent module, just like services.
- Each endpoint should have its own folder under `include/`, `src/`, and `tests/`, not grouped under `ApiModule` or any other shared folder.
- For example, for an endpoint named `EndpointHello`:
    - `include/EndpointHello/EndpointHello.h`
    - `src/EndpointHello/EndpointHello.cpp`
    - `tests/EndpointHelloTest/` (with its own test cases in `cases/`)
- This structure should be used for all endpoints (e.g., `EndpointA`, `EndpointB`, etc.) to maximize decoupling and modularity.
- All test cases for endpoints must be placed in their respective test module folders, following the `cases/` subfolder convention.
- Update all includes, build files, and documentation to reflect this structure when adding or refactoring endpoints.

**Rationale:**
- This approach maximizes decoupling, modularity, and maintainability, making it easy to add, test, and refactor endpoints independently.

## Endpoint Generation Script

- Use the `scripts/create_endpoint.sh` script to generate new endpoint modules automatically
- Usage: 
  - Create: `./scripts/create_endpoint.sh create EndpointName` (must start with "Endpoint" followed by a capitalized name)
  - Remove: `./scripts/create_endpoint.sh remove EndpointName` (removes an existing endpoint with confirmation)
- The script creates the complete modular structure:
  - `include/EndpointName/EndpointName.h`
  - `src/EndpointName/EndpointName.cpp` 
  - `tests/EndpointNameTest/` with test cases and Makefile
- Generated tests include **auto-registration cross-check** that verifies:
  - The endpoint module is automatically registered with ApiModule
  - The specific endpoints (e.g., `/endpoint:GET`) are properly registered
  - All registered modules can be instantiated and used correctly
- Follow the script's instructions to implement your endpoints and update test assertions
- Always test the generated endpoint with `make test-run-EndpointNameTest`
- Use remove function to clean up unwanted endpoints safely

**Rationale:**
- Ensures consistent endpoint structure across the project
- Reduces boilerplate and setup time for new endpoints
- Automatically follows all established conventions and standards
- Provides safe removal with confirmation to prevent accidental deletion
- **Cross-check tests guarantee auto-registration works correctly and catch integration issues early**

## Service Generation Script

- Use the `scripts/create_service.sh` script to generate new service modules automatically
- Usage:
  - Create: `./scripts/create_service.sh create ServiceName` (must start with "Service" followed by a capitalized name)
  - Remove: `./scripts/create_service.sh remove ServiceName` (removes an existing service with confirmation)
- The script creates the complete modular structure:
  - `include/ServiceName/ServiceName.h`
  - `src/ServiceName/ServiceName.cpp`
  - `tests/ServiceNameTest/` with test cases and Makefile
- Generated tests include **IoC registration verification** that verifies:
  - The service module is automatically registered with IoCContainer
  - The service can be resolved and instantiated correctly
  - All registered services work with dependency injection
- Follow the script's instructions to implement your service logic and update test assertions
- Always test the generated service with `make test-run-ServiceNameTest`
- Use remove function to clean up unwanted services safely

**Rationale:**
- Ensures consistent service structure across the project
- Reduces boilerplate and setup time for new services
- Automatically follows all established conventions and standards
- Provides safe removal with confirmation to prevent accidental deletion
- **IoC registration tests guarantee dependency injection works correctly and catch integration issues early**


# AI/Assistant Guidance: Evolving Project Conventions

Whenever the user expresses a new idea, workflow, or coding/build convention during development, the AI assistant should ask the user whether they want to add this idea to `copilot-instructions.md` for future reference and consistency. This ensures that evolving best practices and project-specific rules are captured and documented in a single, authoritative place.

## Test File Type Standardization

- For all test cases that require file input/output, use binary file types instead of `.txt` files. This applies to any test that interacts with files, ensuring consistency and clarity in test data handling.
- Avoid using `.txt` files for new or updated tests; prefer binary files (e.g., `.bin`) to standardize file-based testing across the project.

**Rationale:**
- Using binary files for tests ensures that file handling logic is robust and not dependent on text encoding or formatting, and it provides a consistent approach for all contributors.

**Example:**

- If the user proposes a new test structure, build rule, or coding style, the assistant should confirm: “Do you want to add this convention to `copilot-instructions.md`?”

This helps keep the project’s standards up to date and ensures all contributors and the AI follow the latest agreed practices.
## Makefile and Build System Standards

To ensure reliable, maintainable, and portable builds, all Makefiles in this project must follow these best practices:

- **Compiler Selection:**
    - Always use `g++` to compile `.cpp` (C++) files.
    - Always use `gcc` to compile `.c` (C) files.

- **Object and Dependency Files:**
    - Compile all source files to object files (`*.o`) in a dedicated `obj/` directory, never in the source directory.
    - Generate dependency files (`*.d`) for each source file, also in `obj/`.

- **Binary Output:**
    - Place all test and application executables in a dedicated `bin/` directory.

- **Clean Rule:**
    - Always provide a `clean` target that removes all object files, dependency files, and binaries (e.g., `rm -rf obj/* bin/*`).

- **Best Practices:**
    - Use pattern rules to avoid code duplication for compiling sources.
    - Use variables for source, object, and binary lists.

- **Tab Consistency in Makefiles:**
    - **All Makefile recipe lines (commands) must use a real tab character, not spaces.**
    - This is required by `make` and ensures consistent, portable builds.
    - When generating or editing Makefiles (AI or human), always use a real tab (not spaces) at the start of each command line in a recipe.
    - Example: `\tmkdir -p $(OBJDIR)` (where `\t` is a real tab character, not spaces).


- One file per class: each class should have its own header (.h) and source (.cpp) file, named after the class.
- Namespace per feature: group related classes and functions in a namespace named after the feature/module.

# AI Coding Instructions for CppBoilerplate
This document provides guidelines for AI-assisted coding in the CppBoilerplate project. It includes coding standards, architectural principles, and test-driven development (TDD) practices to ensure high-quality, maintainable code.---

## Architecture: Modular Monolithic

This project follows a **modular monolithic architecture**:

- All features and modules are part of a single codebase and binary, but are organized into clear, self-contained modules.
- Each module has its own directory under `src/` and `include/`, with separate tests under `tests/`.
- Modules communicate via well-defined interfaces and dependency injection, not global state.
- Shared utilities and interfaces are placed in dedicated shared modules.
- This approach enables high cohesion, low coupling, and easier maintainability while avoiding the complexity of microservices.
- **Modular Monolithic Architecture:** Each module should have its own test suite, with object files in `obj/` and test executables in `bin/`.



## Namespace Usage

- Always define a namespace for each feature or module. The namespace should match the feature/module name and group all related classes and functions.
- Do not use `using namespace` in header files. In implementation files, prefer explicit namespace qualification for clarity and to avoid symbol conflicts.

## Microsoft C++ Coding Style

- Use PascalCase for class names, structs, enums, and namespaces.
- Use camelCase for variable names, function parameters, and local functions.
- Use ALL_CAPS for macros and compile-time constants.
- Indent with tabs, each tab 4 spaces.
- Place opening braces on the same line as the declaration (K&R style).
- Use explicit types (avoid `auto` except for lambdas and iterators).
- Prefer smart pointers (`std::unique_ptr`, `std::shared_ptr`) over raw pointers.
- Use `const` wherever possible for variables, parameters, and member functions.
- Group related declarations and functions together logically.
- Add comments to public interfaces and complex or non-obvious logic.

- Design code for easy unit testing and minimal dependencies.
- Favor dependency injection and clear interfaces for all modules to enable mocking and isolation in tests.
- Avoid global state and side effects; always pass dependencies explicitly.

Refer to the [Microsoft C++ Coding Guidelines](https://learn.microsoft.com/en-us/cpp/cpp/cpp-coding-guidelines) for more details.

- One file per class: each class should have its own header (.h) and source (.cpp) file, named after the class.
- Namespace per feature: group related classes and functions in a namespace named after the feature/module.


**Remember:** For any action requiring changes to development code, you must strictly follow TDD. Write test cases first, always. This is non-negotiable and critical to the project's success.

## Build System Standards

### Test Script Output System (First Principles - KISS)

The test script follows first principles design with exactly **3 essential modes**:

#### Default Mode (Tool/LLM-Friendly)
- **Ultra-concise output** optimized for AI context processing (90% token reduction)
- **Structured format**: `TEST=OK BUILD=OK RUN=OK TIME=42s`
- **Essential information only** for maximum context efficiency
- **Usage**: `./scripts/test.sh (default mode)`

#### Human Mode (Human-Friendly)
- **Emojis and celebration** 🔧🧪🚀✅ with colorful, descriptive output
- **Comprehensive feedback** with detailed test reporting and progress indicators
- **Celebration summary** upon successful completion
- **Usage**: `./scripts/test.sh human` 

#### Debug Mode (Full Visibility)
- **Show everything** with full compiler output and detailed logs
- **Complete transparency** for troubleshooting and deep debugging
- **All operations visible** with `[DEBUG]`, `[SUCCESS]`, `[ERROR]` prefixes
- **Usage**: `./scripts/test.sh debug`

#### Key Benefits (First Principles)
- **KISS (Keep It Simple, Stupid)**: Only 3 modes instead of complex 5-mode system
- **Clear purpose differentiation**: Each mode serves exactly one clear purpose
- **Context efficiency**: --minimal mode provides 90% token reduction for AI workflows
- **No confusion**: Mode names clearly indicate their intended use
- **Essential functionality only**: No complexity bloat or theoretical optimizations

#### Examples
```bash
# Default: Ultra-concise for tools/LLMs (90% token reduction)
./scripts/test.sh 
# Output: TEST=OK BUILD=OK RUN=OK TIME=42s

# human: Human-friendly with emojis and celebration
./scripts/test.sh minimal
# Output: 🔧 Running tests...
#         🧪 Running comprehensive test suite...
#         ✅ All tests passed
#         🎉 All checks complete! Your code is working perfectly!

# Debug: Show everything for troubleshooting
./scripts/test.sh debug
# Output: [DEBUG] Running tests with full output...
#         [DEBUG] Building with full compiler output...
#         [SUCCESS] All operations completed successfully

# Build system still supports VERBOSE control for individual commands
make debug VERBOSE=minimal         # Ultra-minimal build output
make debug VERBOSE=human           # Human-friendly build output
make debug VERBOSE=debug           # Debug-level build output
```

**Rationale:** First principles analysis eliminated unnecessary complexity. The simplified 3-mode system provides exactly what developers and AI assistants actually need, without confusing theoretical optimizations. Each mode has a clear, single purpose and delivers exactly what its name promises.

### Makefile Requirements

To ensure reliable, maintainable, and portable builds, all Makefiles in this project must follow these best practices:

#### Compiler Selection
- Always use `g++` to compile `.cpp` (C++) files
- Always use `gcc` to compile `.c` (C) files

#### File Organization
- Compile all source files to object files (`*.o`) in a dedicated `obj/` directory, never in the source directory
- Generate dependency files (`*.d`) for each source file, also in `obj/`
- Place all test and application executables in a dedicated `bin/` directory

#### Tab Consistency
- **All Makefile recipe lines (commands) must use a real tab character, not spaces**
- This is required by `make` and ensures consistent, portable builds
- When generating or editing Makefiles (AI or human), always use a real tab (not spaces) at the start of each command line in a recipe
- Example: `\tmkdir -p $(OBJDIR)` (where `\t` is a real tab character, not spaces)

#### Best Practices
- Always provide a `clean` target that removes all object files, dependency files, and binaries (e.g., `rm -rf obj/* bin/*`)
- Use pattern rules to avoid code duplication for compiling sources
- Use variables for source, object, and binary lists
- Use `-MMD -MP` for dependency generation and `-include obj/*.d` for incremental builds

### Test Makefile Standards

- Follow all main Makefile standards: always use `g++` for `.cpp` and `gcc` for `.c` files, never mix compilers for the same file type
- Compile all test and implementation sources to object files (`*.o`) and dependency files (`*.d`) in `obj/`, never in the source or test code directories
- Place all test executables in `bin/` inside the test module folder (e.g., `tests/ModuleNameTest/bin/`)
- Never pollute the main build output with test artifacts; keep all test objects, binaries, and dependencies isolated in the test module's `obj/` and `bin/` folders
- Always use `mkdir -p $(dir $@)` in every object/dependency rule to ensure all parent directories are created, especially for sources outside the test directory

- Organize object files by origin inside each test module for clarity and reliable pattern matching:
  - `obj/test/...` for test sources (`cases/*.cpp`, `TestMain.cpp`)
  - `obj/src/...` for project sources compiled for the test
  - `obj/external/...` for external library sources
- GNU Make pattern compliance: use a single `%` in the target. Recommended generic rules:
  - `$(OBJDIR)/src/%.o: $(ROOTDIR)/src/%.cpp`
  - Flexible external rule: `$(OBJDIR)/external/%.o: | $(OBJDIR)` with `$(eval)` to handle any path/extension
- Each test Makefile must provide `run`, `clean`, and `debug-config` targets. `debug-config` should print: `MODULE_NAME`, `PRIMARY_MODULE`, `DEPENDENCIES`, `DEP_NAMES`, `DEP_SOURCES`, `DEP_OBJECTS`, and, if present, `EXTERNAL_SOURCES`, `EXT_OBJECTS`, `EXTERNAL_INCLUDES`, `EXTERNAL_LIBS`, plus `TEST_OBJS` and `TEST_BIN`.
- Include Google Mock in test builds in addition to Google Test:
  - Add `-Iexternal/googletest/googlemock/include` to include paths
  - Link with `-lgmock -lgmock_main` alongside GTest

### Test Case Organization

- For every test module (e.g., `tests/ModuleNameTest/`), all test case `.cpp` files **must** be placed in a `cases/` subfolder
- The module's `Makefile` must use a glob pattern (`cases/*.cpp`) to automatically include all test case files in that folder as sources and objects
- Only the test runner (e.g., `TestMain.cpp`) should remain at the root of the test module folder
- To add a new test, **always** create a new `.cpp` file in the `cases/` folder—no Makefile changes required

### Flexible Makefile System

All test modules now use a **flexible Makefile template** that simplifies dependency management and follows consistent patterns:

#### Configuration-Based Dependencies
- Each test Makefile has a clear **CONFIGURATION SECTION** at the top where dependencies are specified declaratively
- No need to manually write compilation rules or object file patterns
- Dependencies are specified in a simple list format: `DEPENDENCIES = ServiceA ServiceB Logger`
- Special folder structures use the format: `ModuleName:FolderName` (e.g., `ApiModule:ApiModule`)

#### Key Features
- **Easy dependency management**: Just add module names to the `DEPENDENCIES` variable
- **Automatic compilation rules**: Generic patterns handle any module type (C++17 standard)
- **Debug support**: Use `make debug-config` to verify dependency resolution
- **Flexible external library support**: Use `EXTERNAL_SOURCES`, `EXTERNAL_INCLUDES`, and `EXTERNAL_LIBS` for maximum flexibility
- **Enhanced testing**: Includes both Google Test and Google Mock support
- **Consistent structure**: All Makefiles follow the same template
- **GNU Make compliance**: Generic pattern rules must use a single `%` in the target (multi-`%` targets are not allowed)
- **Object file organization**: Enforce `obj/test`, `obj/src`, and `obj/external` layout for clarity and reliability

#### Usage Examples

**Simple service with IoC dependency:**
```makefile
MODULE_NAME = ServiceA
DEPENDENCIES = IoCContainer
```

**Endpoint with API modules:**
```makefile
MODULE_NAME = EndpointUser
DEPENDENCIES = ApiModule:ApiModule
```

**Complex module with multiple dependencies and external libraries:**
```makefile
MODULE_NAME = DatabaseManager
DEPENDENCIES = Logger IoCContainer
EXTERNAL_SOURCES = external/sqlite/src/sqlite3.c external/mongoose/src/mongoose.c
EXTERNAL_INCLUDES = -Iexternal/sqlite/include -Iexternal/mongoose/include  
EXTERNAL_LIBS = -lssl -lcrypto
```

**Flexible external source paths:**
```makefile
MODULE_NAME = NetworkModule
DEPENDENCIES = Logger
EXTERNAL_SOURCES = /usr/local/src/libcurl.cpp ../shared/network_utils.cpp
EXTERNAL_INCLUDES = -I/usr/local/include -I../shared/include
EXTERNAL_LIBS = -L/usr/local/lib -lcurl -lpthread
```

#### Debugging Dependencies
Use the built-in debug target to verify your configuration:
```bash
cd tests/ModuleNameTest
make debug-config
```

#### Latest Flexible External Dependency Configuration

The current Makefile template supports maximum flexibility for external dependencies through three dedicated variables:

**EXTERNAL_SOURCES Configuration:**
```makefile
# Examples of flexible external source specification:
EXTERNAL_SOURCES = external/mongoose/src/mongoose.c external/sqlite/sqlite3.c
EXTERNAL_SOURCES = /usr/local/src/mylib.cpp ../shared/utils.cpp
EXTERNAL_SOURCES = $(wildcard external/*/src/*.c) $(wildcard external/*/src/*.cpp)
```

**EXTERNAL_INCLUDES Configuration:**
```makefile
# Examples of external include directories:
EXTERNAL_INCLUDES = -Iexternal/mongoose/include -Iexternal/sqlite/include
EXTERNAL_INCLUDES = -I/usr/local/include -I../shared/include
EXTERNAL_INCLUDES = -I$(ROOTDIR)/external/googletest/googletest/include -I$(ROOTDIR)/external/googletest/googlemock/include
```

**EXTERNAL_LIBS Configuration:**
```makefile
# Examples of external library linking:
EXTERNAL_LIBS = -Lexternal/foo/lib -lfoo -lbar
EXTERNAL_LIBS = -L/usr/local/lib -lsqlite3 -lpthread
EXTERNAL_LIBS = -L$(ROOTDIR)/external/googletest/build/lib -lgtest -lgtest_main -lgmock -lgmock_main
```

**Key Benefits:**
- **Any Source Path**: External sources can be from any location, not restricted to `external/` folder
- **Mixed Extensions**: Supports both `.cpp` and `.c` files automatically
- **Automatic Compilation**: Uses `$(eval)` technique to dynamically determine source file and compiler
- **Standard Integration**: Seamlessly integrates with existing `CXXFLAGS`, `CFLAGS`, and `LDLIBS` variables
- **Debug Support**: All external configuration is displayed in `make debug-config` output

#### Enhanced Test Management System

The project includes a comprehensive test management system accessible both from individual test modules and from the project root:

**Individual Test Management (from test module directory):**
- `make test` - Build and run tests for the specific module
- `make clean` - Clean module-specific artifacts  
- `make debug-config` - Display dependency resolution details
- Standard build targets (`all`, `clean`, etc.)

**Project-Wide Test Management (from project root):**
- `make test-help` - Display comprehensive help with all available test targets
- `make test-stats` - Show project statistics (modules, test files, lines of code)
- `make test-run-<ModuleName>` - Build and run tests for a specific module
- `make test-make-<ModuleName>` - Build tests only for a specific module  
- `make test-clean-<ModuleName>` - Clean artifacts for a specific module
- `make test-debug-<ModuleName>` - Show debug configuration for a specific module
- `make test` - Run all tests in the project
- `make test-clean` - Clean all test artifacts

**Error Handling and User Experience:**
- Intelligent error messages with module name suggestions for typos
- Detailed pass/fail reporting with test counts and execution times
- Comprehensive help system with usage examples
- Project statistics including module counts and test coverage metrics

**Rationale:** The flexible Makefile system with comprehensive external dependency support eliminates boilerplate, reduces errors, ensures consistency across all test modules, and makes dependency management trivial for developers. The latest external configuration approach provides maximum flexibility for any source path, mixed file types, and seamless integration. The enhanced test management system provides a seamless experience for both individual module development and project-wide testing operations.

### Legacy Makefile Example (for reference)
```makefile
# Modern, robust Makefile for unit tests (Google Test)
ROOTDIR = ../..
-include $(ROOTDIR)/Compiler.build
CXX = g++
CC = gcc
CXXFLAGS = -std=$(CXX_STANDARD) -I$(ROOTDIR)/include -I$(ROOTDIR)/external/googletest/googletest/include -g -Wall -Wextra -MMD -MP
CFLAGS = -I$(ROOTDIR)/include -g -Wall -Wextra -MMD -MP
GTEST_LIBS = -L$(ROOTDIR)/external/googletest/build/lib -lgtest -lgtest_main

OBJDIR = obj
BINDIR = bin

TEST_SRC = cases/*.cpp TestMain.cpp $(ROOTDIR)/src/ModuleName/ModuleName.cpp
TEST_OBJS = $(patsubst cases/%.cpp,$(OBJDIR)/%.o,$(wildcard cases/*.cpp)) $(OBJDIR)/TestMain.o $(OBJDIR)/ModuleName.o
TEST_DEPS = $(patsubst %.cpp,$(OBJDIR)/%.d,$(notdir $(basename $(wildcard cases/*.cpp)))) $(OBJDIR)/TestMain.d $(OBJDIR)/ModuleName.d
TEST_BIN = $(BINDIR)/ModuleNameTest

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Pattern rule for C++ test cases in cases/
$(OBJDIR)/%.o: cases/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for the test runner
$(OBJDIR)/TestMain.o: TestMain.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Explicit rule for sources outside test dir
$(OBJDIR)/ModuleName.o: $(ROOTDIR)/src/ModuleName/ModuleName.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

-include $(TEST_DEPS)

clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*
```

### Multithreading for Make

- Always use the `-j` option with `make` to enable multithreaded builds, leveraging multiple CPU cores for faster compilation
- Example: `make -j$(nproc)` where `$(nproc)` dynamically determines the number of available cores
- For systems without `nproc`, specify a reasonable number of threads manually (e.g., `make -j4`)

**Rationale:** Multithreaded builds significantly reduce build times, especially for large projects with many source files.

### Source and Include Management

The project uses an immutable main Makefile design with configurable source management through `Project.build`:

#### Immutable Makefile Design
- **Core Principle**: The main `Makefile` should **never be modified** for project-specific configurations
- **Configuration-Driven**: All customizations are done through `.build` files
- **Generic Logic**: Main Makefile contains only build patterns and logic, no hardcoded paths
- **Separation of Concerns**: Build logic vs. project configuration are completely separated

#### Project.build Configuration File
- **Purpose**: Centralized configuration for source files, include directories, and external libraries
- **Location**: `Project.build` in the project root
- **Integration**: Automatically included by the main Makefile using `-include Project.build`
- **Comprehensive Setup**: Contains all project-specific configurations in one organized file

#### Key Configuration Sections
```makefile
# Source file patterns
EXTERNAL_CPP_SOURCES = $(wildcard external/*/src/*.cpp)
EXTERNAL_C_SOURCES = $(wildcard external/*/src/*.c)

# Include directories
ADDITIONAL_INCLUDES = -Iexternal/mongoose/include

# External libraries
LIB_DIRS += -L/usr/local/lib
LDLIBS += -lsqlite3

# Custom sources
ADDITIONAL_SOURCES += $(YOUR_CUSTOM_SOURCES)
```

#### Usage Examples
```makefile
# Platform-specific configuration:
ifeq ($(ARCH),aarch64)
  PLATFORM_SOURCES += src/platform/arm/*.cpp
  ADDITIONAL_INCLUDES += -Iinclude/platform/arm
  LIB_DIRS += -L/opt/aarch64-toolchain/lib
endif
ADDITIONAL_SOURCES += $(PLATFORM_SOURCES)

# Feature-specific configuration:
ifeq ($(WEBSERVER_BACKEND),mongoose)
  WEBSERVER_IMPL_SOURCES = $(wildcard src/WebServer/impl/mongoose/*.cpp)
  ADDITIONAL_INCLUDES += -Iinclude/WebServer/mongoose
endif
ADDITIONAL_SOURCES += $(WEBSERVER_IMPL_SOURCES)

# Generated/excluded sources:
EXCLUDED_SOURCES = src/deprecated/*.cpp
MAIN_SOURCES := $(filter-out $(EXCLUDED_SOURCES), $(shell find src -name '*.cpp'))
GENERATED_SOURCES = $(wildcard generated/*.cpp)
ADDITIONAL_SOURCES += $(GENERATED_SOURCES)
```

#### When to Use Project.build
- **External Source Patterns**: Custom external library source discovery
- **Platform-Specific Sources**: Architecture or OS-specific source files
- **Generated Sources**: Build-time generated source files
- **Include Directory Customization**: Non-standard include directory patterns
- **Source Exclusions**: Exclude specific files from automatic discovery
- **External Library Dependencies**: All library linking configuration
- **Cross-Compilation**: Architecture-specific configurations

#### Best Practices
- **Never Modify Main Makefile**: All project customizations go in `Project.build`
- **Document Changes**: Comment your customizations in `Project.build`
- **Use Conditional Logic**: Platform/feature-specific configurations with `ifeq` statements
- **Incremental Configuration**: Use `+=` to append to existing variables
- **Organize by Section**: Keep sources, includes, and libraries in their respective sections

**Rationale:** The immutable Makefile design with consolidated `Project.build` ensures that core build logic remains stable and reusable while providing complete flexibility through a single, well-organized configuration file. This approach eliminates redundancy, reduces configuration files, and makes the build system more maintainable and portable across different projects.

### Cross-Compilation and Docker Support

The project provides comprehensive cross-compilation support with Docker-aware platform detection through `Platform.build`:

#### Platform.build Configuration File
- **Purpose**: Handles platform detection, cross-compilation setup, and Docker containerized builds
- **Location**: `Platform.build` in the project root
- **Integration**: Included first by the main Makefile to establish platform context
- **Docker Support**: Automatically detects and adapts to Docker build environments

#### Cross-Compilation Methods
```bash
# Native compilation (auto-detects current architecture)
make

# Traditional cross-compilation
CROSS_COMPILE_PREFIX=aarch64-linux-gnu- make

# Docker cross-compilation with environment variable
docker run --rm -v $(pwd):/workspace -e DOCKER_CROSS_COMPILE=aarch64-linux-gnu- cross-compiler make

# Docker buildx multi-platform builds
docker buildx build --platform linux/arm64,linux/amd64 .
```

#### Docker Environment Detection
- **DOCKER_CROSS_COMPILE**: Override cross-compilation prefix in Docker
- **DOCKER_BUILDPLATFORM**: Automatic platform detection for buildx
- **Vendor Detection**: Distinguishes between native, cross, docker, and buildx builds
- **Build Directory Organization**: Creates architecture-specific directories (e.g., `x86_64-native`, `aarch64-docker`)

#### Supported Architectures
- **aarch64**: ARM 64-bit (common in modern ARM processors, Apple Silicon)
- **arm**: ARM 32-bit (embedded systems, older ARM devices)  
- **x86_64**: Intel/AMD 64-bit (standard desktop/server architecture)
- **Auto-detection**: Falls back to `uname -m` for unknown architectures

#### Build Name Examples
- `x86_64-native`: Native x86_64 compilation
- `aarch64-cross`: Cross-compilation to ARM64
- `arm-docker`: ARM compilation in Docker container
- `x86_64-buildx`: Docker buildx multi-platform build

**Rationale:** Docker-aware cross-compilation support enables seamless CI/CD integration, consistent build environments across different development machines, and simplified multi-architecture deployments common in modern containerized applications.

---

## Test-Driven Development (TDD)

### Core Principle

**For any action requiring changes to development code, you must strictly follow TDD. Write test cases first, always. This is non-negotiable and critical to the project's success.**

### TDD Workflow

1. **Define the Requirement**
   - Clearly state the feature or bug to address before writing any code
   - Write unit tests before or alongside production code
   - For every new feature or bugfix, add or update tests describing expected behavior

2. **Write a Failing Test**
   - Clean the existing build and test artifacts before making changes:
     - Run `make clean` to remove all build artifacts
     - Run `make test-clean` to remove all test artifacts
   - Create a new test in `tests/ModuleNameTest/cases/`
   - Use Google Test; name test files and cases in PascalCase
   - Focus on the public interface and expected behavior
   - Place all test-only code (mocks, helpers) inside the module's test folder, never in production `src/` or `include/`

3. **Build and Run Tests**
   - After every major code or build change, always run the relevant module's tests to catch errors early:
     - Clean test artifacts: `make test-clean-<ModuleName>` (from project root) or `make clean` (from test module)
     - Build tests only: `make test-make-<ModuleName>` (from project root) or standard build targets (from test module)
     - Build and run tests: `make test-run-<ModuleName>` (from project root) or `make test` (from test module)
     - Debug configuration: `make test-debug-<ModuleName>` (from project root) or `make debug-config` (from test module)
   - Use `make test-help` (from project root) to see all available test management targets
   - Use `make test-stats` (from project root) to view project statistics and test coverage metrics
   - Confirm the new test fails (red) before proceeding to implementation
   - Only run the full test suite (`make test-clean`, `make debug`, `make test`) if explicitly requested

4. **Implement the Minimal Code**
   - Write just enough code in `src/ModuleName/ModuleName.cpp` to make the test pass
   - Follow modern C++ and project style guidelines
   - Ensure all public APIs and logic are covered by tests; avoid tightly coupling code to I/O or system state

5. **Rebuild and Retest**
   - Run the test suite again after any significant code, build, or dependency change
   - Ensure the new test passes (green) and no regressions occur
   - Use Google Test for all C++ unit tests, and ensure all code is built with debug symbols (`-g`)

6. **Refactor**
   - Clean up code and tests for clarity, modularity, and maintainability
   - Use smart pointers, dependency injection, and standard algorithms
   - Refactor tests and production code together to maintain clarity, coverage, and maintainability

7. **Repeat**
   - For each new requirement or bug, repeat the cycle
   - Maintain high test coverage and code quality
   - Use mocks and dependency injection to isolate tests from real system/network state

8. **Document**
   - Add comments to public interfaces and complex logic
   - Clearly document the purpose of each test
   - Name test cases and mocks clearly

### Enforcement Rules

- **No direct implementation without a failing test**
- **No skipping of test validation after changes**
- **No merging of code that has not been tested**

### Testing Best Practices

- Use Google Test for all C++ unit tests
- Place test cases in `cases/` subfolder for scalability
- Generate both basic functionality tests and integration tests (IoC/ApiModule registration)
- Use mocks and dependency injection to isolate tests from real system/network state
- Avoid tests that depend on external resources unless explicitly required

**Rationale:** Strict TDD ensures predictable and reliable code behavior, early detection of bugs and regressions, and a maintainable and scalable codebase.

### Test Isolation Principles

**MANDATORY TEST DECOUPLING** - All tests must be completely isolated from other modules:

#### Core Principle: Test Only What You're Testing

**RULE**: Each test module should **ONLY** depend on the module being tested. Zero external module dependencies.

#### ❌ WRONG: Tightly Coupled Tests
```cpp
// DON'T DO THIS - test depends on Logger module
#include <Logger/Logger.h>
#include <Logger/ILogger.h>

TEST(IocContainerTest, ServiceResolution) {
    auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);  // External dependency!
    container.registerInstance<logger::ILogger>(logger);               // Domain coupling!
    
    auto resolved = container.resolve<logger::ILogger>();               // Business logic leak!
    resolved->logInfo("Test message");                                  // Not testing container!
}
```

**Problems:**
- Changes to Logger **break IoC Container tests**
- Tests fail when Logger has bugs
- Not testing IoC Container functionality - testing Logger integration
- Build dependencies create coupling

#### ✅ CORRECT: Isolated Tests
```cpp
// CORRECT - completely generic, no domain knowledge
class IServiceA {
public:
    virtual ~IServiceA() = default;
    virtual int getValue() = 0;
    virtual void setValue(int value) = 0;
    virtual void increment() = 0;
};

class ServiceAImpl : public IServiceA {
private:
    int _value;
public:
    ServiceAImpl(int initial = 42) : _value(initial) {}
    int getValue() override { return _value; }
    void setValue(int value) override { _value = value; }
    void increment() override { _value++; }
};

TEST(IocContainerTest, ServiceResolution) {
    auto service = std::make_shared<ServiceAImpl>(100);                // Generic service
    container.registerInstance<IServiceA>(service);                   // Tests container mechanics
    
    auto resolved = container.resolve<IServiceA>();                    // Tests resolution
    EXPECT_EQ(resolved->getValue(), 100);                             // Tests functionality
    resolved->increment();                                             // Tests usability
    EXPECT_EQ(resolved->getValue(), 101);                             // Verifies state
}
```

**Benefits:**
- Tests **only** IoC Container functionality
- Zero external dependencies
- Works with **any** service type (banking, gaming, utilities)
- Changes to other modules **never** break these tests

#### Implementation Guidelines

1. **Zero External Dependencies**:
   ```makefile
   # Makefile should have empty dependencies
   DEPENDENCIES =  # No external modules!
   ```

2. **Generic Test Services**:
   ```cpp
   // Use domain-agnostic interfaces
   class IServiceA { /* generic operations */ };
   class IServiceB { /* different generic operations */ };
   
   // NO business domain concepts:
   // ❌ ILogger, IWebServer, IPaymentProcessor
   // ✅ IServiceA, IServiceB, IServiceC
   ```

3. **Test Container Mechanics, Not Business Logic**:
   ```cpp
   // Test what the CONTAINER does:
   ✅ Service registration works
   ✅ Service resolution works  
   ✅ Thread safety works
   ✅ Error handling works
   ✅ Service lifecycle works
   
   // DON'T test what SERVICES do:
   ❌ Logging functionality
   ❌ HTTP request handling
   ❌ Database operations
   ```

#### When Isolation Is Required

**ALWAYS isolate when testing**:
- Core infrastructure (IoC Container, Router, etc.)
- Utility classes and algorithms
- Data structures and collections
- Any component meant to work with multiple service types

**Integration tests are separate**:
- Create dedicated integration test modules for cross-module testing
- Use `tests/IntegrationTest/` for real service interactions
- Keep unit tests and integration tests clearly separated

#### Test Isolation Checklist

Before committing tests, verify:
- [ ] Makefile has `DEPENDENCIES =` (empty)
- [ ] No `#include` of other project modules
- [ ] Uses generic test services (`IServiceA`, `ServiceAImpl`)
- [ ] Tests component mechanics, not business logic
- [ ] All tests pass with only target module built
- [ ] No domain-specific concepts in test code

**Remember**: The best test is one that tests **exactly one thing** in **complete isolation**. If your IoC Container tests break when you change the Logger, you're not testing the IoC Container - you're testing the integration.

## **CRITICAL: Testing Strategy Guidelines**

### **Mock vs Isolation Decision Framework**

**FUNDAMENTAL RULE**: **Mock your business logic, isolate your infrastructure.**

#### **When to Use MOCKING:**
```cpp
// ✅ MOCK: Business logic, domain objects, complex algorithms
TEST(ApiRouterTest, RoutesToCorrectEndpoint) {
    MockEndpoint mockEndpoint;
    EXPECT_CALL(mockEndpoint, handleRequest(_)).Times(1);
    apiRouter.registerEndpoint("/api/users", mockEndpoint);
    apiRouter.handleRequest("/api/users", "GET", ...);
}
```

**Mock these types:**
- **Business logic classes** (ApiRouter, Services, Controllers)
- **Domain objects** with complex behavior and state
- **Classes you own and control** completely
- **Multi-step algorithms** and workflows
- **Classes with significant logic** to test

#### **When to Use ISOLATION:**
```cpp
// ✅ ISOLATE: Infrastructure, thin wrappers, external library facades
TEST(WebServerTest, StartOperationDoesNotCrash) {
    WebServer webServer("127.0.0.1", 8080);
    EXPECT_NO_THROW(webServer.start());  // Test our interface, not mongoose
    EXPECT_EQ(webServer.getBindAddress(), "127.0.0.1");
}
```

**Isolate these types:**
- **Infrastructure wrappers** (WebServer, Database connections)
- **External library facades** (mongoose, SQLite, curl wrappers)
- **Simple data holders** and configuration objects
- **System boundary classes** (file I/O, network, OS interfaces)
- **Thin wrappers** where most logic lives in external libraries

#### **Why This Distinction Matters:**

**Infrastructure Code Reality:**
```cpp
// WebServer is mostly mongoose + thin interface
class WebServer {
    bool start() { return mongoose_start(); }  // Thin wrapper
    void setHandler(Handler h) { handler = h; } // Simple storage
};

// Mocking mongoose internals = testing the wrong thing
❌ EXPECT_CALL(mongoose, mg_http_listen(addr, port, handler));
✅ EXPECT_EQ(webServer.getBindAddress(), "127.0.0.1");
```

**Business Logic Reality:**
```cpp
// ApiRouter has complex routing logic we own
class ApiRouter {
    bool handleRequest(path, method, body, response) {
        auto key = createEndpointKey(path, method);  // Our logic
        auto endpoint = findEndpoint(key);           // Our logic
        return endpoint->handle(request);            // Our orchestration
    }
};

// We want to test OUR routing logic, not endpoint implementation
✅ EXPECT_CALL(mockEndpoint, handle(_)).WillReturn(true);
❌ WebServer webServer; // Don't test real HTTP over network
```

#### **Testing Value Assessment:**

**High-Value Tests (Write These):**
- ✅ **Interface contracts** - Does the API work as promised?
- ✅ **Error handling** - How does it fail gracefully?
- ✅ **Configuration** - Are settings stored/applied correctly?
- ✅ **Business logic** - Do algorithms produce correct results?

**Low-Value Tests (Avoid These):**
- ❌ **Implementation details** - Which internal functions get called?
- ❌ **External library behavior** - Does mongoose parse HTTP correctly?
- ❌ **Integration complexity** - Real network calls in unit tests
- ❌ **Timing dependencies** - Tests that fail due to race conditions

#### **Practical Guidelines:**

**For Infrastructure (WebServer, Logger, Database):**
1. **Test configuration and lifecycle** - Does it start/stop safely?
2. **Test your interface contract** - Do methods return expected values?
3. **Test error handling** - Does it fail gracefully with bad input?
4. **Don't test the library** - Trust mongoose to parse HTTP correctly

**For Business Logic (ApiRouter, Services, Domain):**
1. **Mock dependencies** - Control what collaborators do
2. **Test decision logic** - Does it route/process/calculate correctly?
3. **Test edge cases** - How does it handle unusual inputs?
4. **Test state changes** - Does it update internal state correctly?

#### **Architecture Testing Strategy:**

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Integration   │    │  Business Logic │    │ Infrastructure  │
│      Tests      │    │   (Unit Tests)  │    │ (Isolated Tests)│
├─────────────────┤    ├─────────────────┤    ├─────────────────┤
│ ApiRouter +     │    │ ApiRouter       │    │ WebServer       │
│ WebServer +     │    │ (mocked deps)   │    │ (no network)    │
│ Real HTTP       │    │                 │    │                 │
│                 │    │ Services        │    │ Logger          │
│ ┌─┐ ┌─┐ ┌─┐    │    │ (mocked deps)   │    │ (no file I/O)   │
│ │E│ │F│ │F│    │    │                 │    │                 │
│ │2│ │e│ │a│    │    │ Domain Logic    │    │ Database        │
│ │E│ │w│ │s│    │    │ (mocked repos)  │    │ (no real DB)    │
│ └─┘ └─┘ └─┘    │    │                 │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
     Slow                    Fast                   Fast
   Real Network            Mocked Deps           No External Deps
```

**Layer Testing Strategy:**
- **Infrastructure**: Isolated unit tests (fast, no external deps)
- **Business Logic**: Mocked unit tests (fast, controlled dependencies)  
- **Integration**: End-to-end tests (slower, real components)

This approach maximizes test value while minimizing maintenance overhead and execution time.

---

## Development Workflow

### Efficient Debug & Test Workflow

1. **Always work from the project root**
2. **Enhanced test management system:** Use the comprehensive test management commands for streamlined development:
   - **Individual module testing:** `make test-run-<ModuleName>` (builds and runs tests for specific module)
   - **Project overview:** `make test-help` (shows all available test targets with examples)
   - **Project statistics:** `make test-stats` (displays module counts, test files, and lines of code)
   - **Debug configuration:** `make test-debug-<ModuleName>` (shows dependency resolution details)
   - **Batch operations:** `make test` (runs all tests), `make test-clean` (cleans all test artifacts)
3. **Main app workflow:** Use batch commands for rapid feedback:
   - `make debug && make run_debug`
   - Clean & build: `make clean && make debug && make run_debug`
4. **After every change:** Run relevant module tests to catch issues early using the enhanced test targets
5. **Error handling:** System provides intelligent error messages with module name suggestions for typos
6. **Iterative debugging:** If a test fails, fix and rerun up to 5 times, reviewing all output
7. **Escalate if needed:** If problems persist after 5 attempts, escalate with detailed logs and context

**Rationale:**
- **Enhanced user experience**: Comprehensive test management with detailed reporting and error handling
- **Efficiency**: Running only the relevant tests saves time, with project-wide overview when needed
- **Focus**: Detailed feedback and statistics help developers understand project state and progress
- **Scalability**: As the project grows, the enhanced system ensures manageable and informative test operations

### Script and Template Generation Conventions

#### Universal Script Standards

All generation scripts (`create_endpoint.sh`, `create_service.sh`, `create_module.sh`, etc.) must follow consistent patterns:
- Script location resolution: Use the standard pattern to resolve script directory and run from project root
- Argument parsing: Use simple positional arguments: `<create|remove> <ModuleName>`
- Module naming: Validate naming conventions (endpoints start with "Endpoint", services start with "Service")
- Function-based structure: Separate create and remove logic into dedicated functions
- Consistent user experience: Use emojis, clear messaging, and helpful next steps

#### Script Implementation Requirements

- Bash scripts must resolve their own location and run from the project root for predictable relative paths:
  ```bash
  SCRIPTDIR="$(cd -- "$(dirname "$0")" >/dev/null 2>&1; pwd -P)"
  PROJECT_ROOT="$(dirname "$SCRIPTDIR")"
  cd "$PROJECT_ROOT"
  ```
- Use PROJECT_ROOT-relative paths for all script operations (creating/removing sources, headers, tests)
- Do not define ROOTDIR inside scripts. Only define ROOTDIR in generated Makefiles as the path from the test folder to the project root (usually `../..`)
- Ensure generated Makefiles use real tabs at the start of recipe lines (not spaces)
- Ensure generated test Makefiles include Google Mock support (add `-Iexternal/googletest/googlemock/include` and link `-lgmock -lgmock_main`)
- Ensure generated test Makefiles organize objects by origin (`obj/test`, `obj/src`, `obj/external`)

#### Template Quality Standards

Generated code templates must:
- Follow Microsoft C++ coding conventions (PascalCase classes, camelCase variables)
- Use proper namespace conventions (lowercase module names)
- Include comprehensive test coverage with auto-registration verification
- Place test cases in `cases/` subfolder for scalability
- Generate both basic functionality tests and integration tests (IoC/ApiModule registration)

#### Script Documentation Requirements

Each script must provide:
- Clear usage examples in help output
- Descriptive creation and removal confirmations
- Next steps instructions with specific make commands
- Safety confirmations for removal operations

**Rationale:** All generation scripts follow identical patterns for predictable developer experience, reliability, quality, safety, and maintainability.

---

## AI/Assistant Guidance

### Evolving Project Conventions

Whenever the user expresses a new idea, workflow, or coding/build convention during development, the AI assistant should ask the user whether they want to add this idea to `copilot-instructions.md` for future reference and consistency. This ensures that evolving best practices and project-specific rules are captured and documented in a single, authoritative place.

**Example:** If the user proposes a new test structure, build rule, or coding style, the assistant should confirm: "Do you want to add this convention to `copilot-instructions.md`?"

This helps keep the project's standards up to date and ensures all contributors and the AI follow the latest agreed practices.

### Key Reminders

- **Strict adherence to TDD is mandatory** for all development tasks
- **Use differentiated generation approach**:
  - `./scripts/create_module.sh create ModuleName` - Simple by default, add `--interface` or `--pimpl` when needed
  - `./scripts/create_service.sh create ServiceName` - Always includes interface for DI/testing  
  - `./scripts/create_endpoint.sh create EndpointName` - Testable handler methods
  - `./scripts/generate_class_chart.sh` for UML class diagrams and documentation
- **Progressive enhancement principle**: Start simple, add complexity only when requirements justify it
- **Service architecture**: Always use interfaces for dependency injection and mocking
- **Endpoint architecture**: Extract handler logic to testable protected methods
- Use the flexible Makefile system: modify only the DEPENDENCIES line in test Makefiles
- For external dependencies, use `EXTERNAL_SOURCES`, `EXTERNAL_INCLUDES`, and `EXTERNAL_LIBS` for maximum flexibility
- Use `make debug-config` to verify dependency resolution in test modules
- Follow the modular monolithic architecture principles
- Ensure all Makefiles use real tabs for recipe lines
- Place test cases in `cases/` subfolders for scalability
- Use dependency injection and clear interfaces for testability
- Run relevant tests after every significant change
- **Never modify the main Makefile**: Use `Project.build` for all project-specific configuration
- Configure sources, includes, and external libraries through `Project.build` for centralized management
- Use the enhanced test management system: `make test-help` for comprehensive test targets

---

*Refer to the [Microsoft C++ Coding Guidelines](https://learn.microsoft.com/en-us/cpp/cpp/cpp-coding-guidelines) for additional details on coding standards.*