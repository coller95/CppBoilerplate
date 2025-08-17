# CppBoilerplate

A modern C++ modular monolithic project with metaprogramming auto-registration patterns, IoC dependency injection, HTTP API endpoints, and LLM-optimized tooling. Features 90%+ boilerplate reduction through CRTP endpoints and template-based service registration, comprehensive testing with TDD enforcement, and dual-mode output for both human developers and AI agents.

## 🚀 Quick Start

### Bootstrap Fresh Clone

After cloning the repository, run the bootstrap script to set up all dependencies:

```bash
# Standard bootstrap (recommended)
./bootstrap.sh

# With verbose output to see all commands
./bootstrap.sh --verbose

# Download dependencies only (skip building)
./bootstrap.sh --skip-build

# Force rebuild even if dependencies exist
./bootstrap.sh --force
```

**What the bootstrap script does:**
- ✅ Checks system requirements (git, make, gcc/g++, cmake)
- ✅ Downloads Google Test framework (v1.14.0)
- ✅ Builds Google Test libraries
- ✅ Sets up external library structure (mongoose, foo examples)
- ✅ Validates setup with test build
- ✅ Provides next steps for development

### Manual Setup (Alternative)

If you prefer manual setup or the bootstrap script fails:

```bash
# Install system dependencies
sudo apt-get install git make gcc g++ cmake curl  # Ubuntu/Debian
# sudo yum install git make gcc gcc-c++ cmake curl   # RHEL/CentOS

# Download and build Google Test
cd external
git clone --depth 1 --branch v1.14.0 https://github.com/google/googletest.git
cd googletest && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

---

- 🚀 **Auto-Registration Patterns**: CRTP endpoints + IoC services (90%+ boilerplate reduction)
- ⚠️ **TDD Mandatory**: Failing tests first, one class per file, test isolation enforced
- 🤖 **LLM Optimization**: 90-95% token reduction tools (lexer.sh, llm_analysis.sh)
- 🎯 **Dual-Mode Output**: Human-friendly vs AI agent-optimized (minimal/human/debug)
- 🔄 **Modular Monolithic**: Auto-registering modules, high cohesion, zero coupling
- 🧪 **Test Isolation**: Zero external dependencies, mock business logic only
- 📦 **Metaprogramming IoC**: Template auto-registration for services + interfaces
- 🌐 **CRTP Endpoints**: Type-safe auto-registration with clean method naming
- 🛠️ **Generation Scripts**: Progressive complexity (simple/interface/PIMPL patterns)
- 🔍 **Multi-Logger System**: Console + Network + Composite logging

---

## 📁 Project Architecture

```
CppBoilerplate/
├── CLAUDE.md             # LLM agent instructions and TDD rules
├── Makefile              # Immutable core build logic
├── Platform.build        # Cross-compilation and architecture detection
├── Project.build         # Source files, includes, and library configuration
├── Tests.build           # Enhanced test management system
├── include/              # Modular headers by component
│   ├── ApiRouter/        # Auto-registering API endpoint management + CRTP base
│   │   ├── ApiRouter.h
│   │   ├── AutoRegisterEndpoint.h  # CRTP metaprogramming pattern
│   │   └── IEndpointRegistrar.h
│   ├── EndpointHello/    # Example auto-registering HTTP endpoint
│   ├── IocContainer/     # Dependency injection with auto-registration
│   │   ├── AutoRegisterService.h   # Template auto-registration
│   │   ├── IIocContainer.h
│   │   └── IocContainer.h
│   ├── Logger/           # Composite logging (Console + Network + Multi-logger)
│   │   ├── CompositeLogger.h
│   │   ├── ConsoleLogger.h
│   │   ├── ILogger.h
│   │   └── NetworkLogger.h
│   └── WebServer/        # HTTP server abstraction (Mongoose backend)
├── src/                  # Modular implementation by component  
│   ├── main.cpp          # IoC + WebServer + ApiRouter orchestration
│   ├── ApiRouter/        # Auto-registration implementation
│   ├── EndpointHello/    # Example endpoint with CRTP auto-registration
│   ├── IocContainer/     # Dependency injection implementation
│   ├── Logger/           # Multi-logger implementation (3 loggers)
│   └── WebServer/        # WebServer implementation
├── tests/                # TDD-enforced comprehensive test framework
│   ├── Makefile          # Master test coordination
│   ├── ApiRouterTest/    # CRTP auto-registration + endpoint tests
│   ├── EndpointHelloTest/ # Endpoint handler + auto-registration tests
│   ├── IocContainerTest/ # Dependency injection + concurrency tests
│   ├── LoggerTest/       # Multi-logger tests
│   └── WebServerTest/    # HTTP server tests
├── scripts/              # LLM-optimized automation and code generation
│   ├── test.sh           # Multi-mode test automation (minimal/human/debug)
│   ├── create_endpoint.sh # Generate CRTP auto-registering endpoints
│   ├── create_service.sh  # Generate auto-registering IoC services
│   ├── create_module.sh   # Generate modules (simple/interface/PIMPL)
│   ├── lexer.sh          # LLM-optimized code analysis (90% token reduction)
│   ├── llm_analysis.sh   # Standardized LLM analysis tools
│   ├── refactor.sh       # Safe refactoring tools
│   └── debug.sh          # Quick dev cycle: test + build + run
├── external/             # External libraries with source
│   ├── googletest/       # Google Test framework
│   └── mongoose/         # HTTP server library
└── bin/                  # Architecture-specific build output
    └── x86_64-native/    # Example architecture
        ├── debug/        # Debug builds with symbols
        └── release/      # Optimized release builds
```

### Modular Architecture Principles

- **⚠️ TDD Mandatory**: Write failing test FIRST, then implement (RED-GREEN-REFACTOR)
- **⚠️ Test Isolation**: Zero external module dependencies in tests (only IServiceA, IServiceB)
- **One Class Per File**: Mandatory for LLM context efficiency and surgical targeting
- **Auto-Registration**: CRTP endpoints + IoC services eliminate 90%+ boilerplate
- **Metaprogramming**: Template-based registration vs manual lambda wrapping
- **Progressive Complexity**: Simple → Interface → PIMPL enhancement patterns

---

## 🛠️ Building

### Quick Start

Test, build, and run everything (LLM-optimized minimal mode):
```bash
./scripts/test.sh               # Minimal mode: TEST=OK BUILD=OK RUN=OK TIME=42s
```

Human-friendly mode (emojis and colors):
```bash
./scripts/test.sh human         # Human mode: rich output with emojis
```

Full debug output:
```bash
./scripts/test.sh debug         # Debug mode: complete verbose output
```

Quick dev cycle:
```bash
./scripts/debug.sh              # Quick dev: test + build + run debug
```

### Individual Build Commands

**Core builds:**
```bash
make debug              # Build debug version
make release            # Build release version (or just 'make')
make test               # Run all tests
make clean              # Clean build artifacts
```

**TDD workflow:**
```bash
make test-run-ModuleName    # Test specific module (TDD cycle)
make run_debug              # Build and run debug version
```

**LLM tools:**
```bash
./scripts/lexer.sh analyze src/                    # 90% token reduction analysis
./scripts/llm_analysis.sh analyze file.json type  # Standardized analysis
./scripts/refactor.sh extract-class file Class    # Safe refactoring
```

**Module generation:**
```bash
./scripts/create_module.sh create ModuleName --interface --pimpl   # Progressive complexity
./scripts/create_service.sh create ServiceName --interface         # Auto-registering IoC
./scripts/create_endpoint.sh create EndpointName                   # CRTP auto-registration
```

### Build Output Modes

Three output modes optimized for different consumers:

#### Minimal Mode (default for LLM agents)
- 🤖 **90-95% token reduction** for LLM context efficiency
- 📋 Ultra-concise: `TEST=OK BUILD=OK RUN=OK TIME=42s`
- ⚡ Essential status only, perfect for AI processing
- 🔇 Silent compilation, errors/warnings visible

#### Human Mode (--human flag)
- 🎨 Colorful, descriptive output with emojis
- 📝 Full compilation commands visible
- 📊 Rich progress indicators and context
- 🔍 Detailed error messages with suggestions

#### Debug Mode (--debug flag)  
- 🔧 Complete verbose output for troubleshooting
- 📋 Full command traces and build details
- 🔍 Maximum visibility for development debugging
- 📝 All intermediate steps shown

**Examples:**
```bash
# Minimal mode (LLM-optimized):
TEST=OK BUILD=OK RUN=OK TIME=42s

# Human mode:
🔨 Compiling (debug): src/ApiRouter/ApiRouter.cpp
🔗 Linking debug executable...
✅ Successfully built debug version

# Debug mode:
[BUILD] g++ -std=c++17 -g -Wall -Wextra -Iinclude...
[BUILD] LINK debug hello_world
[BUILD] BUILT debug hello_world x86_64-native
```

### Cross-Platform & Docker Support

**Architecture-specific builds:**
```bash
# Native compilation (auto-detects)
make debug

# Cross-compilation
CROSS_COMPILE_PREFIX=aarch64-linux-gnu- make

# Docker cross-compilation
docker run --rm -v $(pwd):/workspace -e DOCKER_CROSS_COMPILE=aarch64-linux-gnu- builder make
```

Build artifacts are organized by architecture: `bin/<arch-vendor>/{debug,release}/`

---

## 🚀 Running

### Quick Run
```bash
make run                # Build and run release version
make run_debug          # Build and run debug version
```

### Manual Execution
After building, run the executable (replace `<arch-vendor>` with your architecture):

```bash
# Release build
./bin/x86_64-native/release/hello_world

# Debug build  
./bin/x86_64-native/debug/hello_world
```

### Example Output

```bash
$ ./bin/x86_64-native/debug/hello_world
🚀 Application starting with IoC + WebServer + ApiRouter orchestration
📦 IocContainer: Auto-registered services via metaprogramming templates
🌐 ApiRouter: Auto-registered endpoints via CRTP patterns:
  - GET /hello (EndpointHello auto-registration)
🔍 Logger: Composite logging (Console + Network) initialized
🚀 WebServer: Mongoose backend started on port 8080
✅ Application ready: http://localhost:8080/hello
Press Enter to exit...
```

### HTTP Endpoints

The application demonstrates auto-registering endpoints via CRTP patterns:

- **GET /hello** - Auto-registered EndpointHello via metaprogramming

Test the auto-registered endpoint:
```bash
curl http://localhost:8080/hello
# Response: Hello, World! (from auto-registered CRTP endpoint)
```

## 🧪 Testing

### ⚠️ TDD MANDATORY Framework

**STRICT RULES** (non-negotiable):
- **Write failing test FIRST**, then implement minimal code to pass
- **NO code without tests** - enforced through development workflow
- **ONE CLASS PER FILE** - mandatory for context efficiency
- **RED-GREEN-REFACTOR** cycle must be followed

### ⚠️ TEST ISOLATION MANDATORY

**RULE**: Zero external module dependencies in tests. Test only what you're testing.

**❌ WRONG**: 
```cpp
#include <Logger/Logger.h>  // External dependency breaks isolation!
```

**✅ CORRECT**:
```cpp
// Generic test services only
class IServiceA { virtual int getValue() = 0; };
class ServiceAImpl : public IServiceA { /*...*/ };
```

### Test Framework Features

- **TDD-Enforced**: Failing tests first, implementation second
- **Test Isolation**: Zero external dependencies (only generic IServiceA, IServiceB)  
- **Auto-Registration Tests**: Verify CRTP endpoints + IoC services register correctly
- **Mock Business Logic**: Mock services/domain, isolate infrastructure
- **Modular Structure**: Each module has isolated test suite in `tests/ModuleNameTest/`

### Quick Testing
```bash
# TDD workflow (most common)
make test-run-ModuleName     # RED: failing test, GREEN: pass, REFACTOR

# All tests
make test                    # Run all tests (human output)
./scripts/test.sh            # Comprehensive test + build + run (minimal mode)
./scripts/test.sh human      # Human-friendly test output
./scripts/test.sh debug      # Full verbose test output

# Testing help
make test-help               # Available test targets and commands
make test-stats              # Project statistics and metrics
```

### Individual Module Testing

From project root:
```bash
make test-run-<ModuleName>    # Build and run tests for specific module
make test-make-<ModuleName>   # Build tests only (no execution)
make test-clean-<ModuleName>  # Clean test artifacts for specific module
make test-debug-<ModuleName>  # Show dependency configuration
```

From test module directory (e.g., `tests/LoggerTest/`):
```bash
make test           # Build and run tests
make clean          # Clean artifacts
make debug-config   # Show dependency resolution
```

### Test Organization Structure

```
tests/
├── ModuleNameTest/
│   ├── Makefile            # Flexible dependency configuration
│   ├── TestMain.cpp        # Test runner
│   ├── cases/              # All test case files
│   │   ├── BasicTest.cpp
│   │   ├── IntegrationTest.cpp
│   │   └── MockTest.cpp
│   ├── bin/                # Test executables
│   └── obj/                # Test object files
```

### Test Makefile Configuration

Each test module uses a flexible configuration system:

```makefile
# Simple service dependency
MODULE_NAME = ServiceA
DEPENDENCIES = IoCContainer

# Complex endpoint with API modules  
MODULE_NAME = EndpointUser
DEPENDENCIES = ApiModule:ApiModule Logger

# External library integration
MODULE_NAME = WebServer
DEPENDENCIES = Logger
EXTERNAL_DEPS = mongoose
```

### Advanced Testing Features

- **Auto-Registration Tests**: Verify IoC container and API module registration
- **Mock Integration**: Google Mock for dependency isolation
- **Error Handling**: Intelligent error messages with module suggestions
- **Batch Operations**: Project-wide test management from root directory
- **Debug Support**: Dependency resolution debugging with `make debug-config`

---

## 🚀 Metaprogramming Auto-Registration

### Before vs After: 90%+ Boilerplate Reduction

**Before (Manual Registration):**
```cpp
// Verbose endpoint registration (15+ lines)
class EndpointUser : public apirouter::IApiModule {
public:
    void registerEndpoints(apirouter::IEndpointRegistrar& registrar) override;
    void handleGetUser(std::string_view path, std::string_view method, 
                       const std::string& requestBody, std::string& responseBody, int& statusCode);
};

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

**After (CRTP Auto-Registration):**
```cpp
// Clean CRTP inheritance
class EndpointUser : public apirouter::AutoRegisterEndpoint<EndpointUser> {
public:
    void registerAvailableMethods(apirouter::IEndpointRegistrar& registrar, const std::string& basePath) override;
    void handleGet(std::string_view path, std::string_view method, 
                   const std::string& requestBody, std::string& responseBody, int& statusCode);
};

// Type-safe registration (3 lines)
void EndpointUser::registerAvailableMethods(apirouter::IEndpointRegistrar& registrar, const std::string& basePath) {
    registerMethod<&EndpointUser::handleGet>(registrar, "/user", "GET");
}

// One-line registration!
namespace {
    static apirouter::AutoRegister<EndpointUser> _autoRegister;
}
```

### Service Auto-Registration

**Before (Manual IoC):**
```cpp
// Manual IoC registration (13+ lines)
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
```

**After (Template Auto-Registration):**
```cpp
// One-line registration!
namespace {
    static ioccontainer::AutoRegister<ServiceUser, IServiceUser> _autoRegister;
}
```

### Benefits

- **85% less boilerplate** for endpoints (no manual lambda wrapping)
- **90%+ less boilerplate** for services (no manual IoC registration)
- **Type safety** - Compile-time method signature verification
- **Clean naming** - `handleGet()` vs `handleGetUser()`
- **Zero runtime overhead** - Static initialization only

---

## 🤖 Automation Scripts

### Comprehensive Test Script

The `scripts/test.sh` script provides comprehensive testing automation with dual-mode output:

```bash
# Human-friendly mode (default)
./scripts/test.sh

# AI-agent silent mode  
./scripts/test.sh agent

# Explicit human mode
./scripts/test.sh human
```

#### Output Modes

- **Human Mode**: Colorful, detailed output with emojis and progress indicators
- **Agent Mode**: Ultra-concise structured output optimized for AI context processing
- **Error Visibility**: Full compilation errors and warnings in both modes

#### Features

- Comprehensive build testing (debug, release, cross-platform)
- Full test suite execution with detailed reporting
- Artifact cleanup and verification
- Performance timing and statistics
- Error detection and reporting

### Module Generation Scripts

Automated module creation with consistent structure:

```bash
# Create new endpoint
./scripts/create_endpoint.sh create EndpointUser

# Create new service
./scripts/create_service.sh create ServiceAuth

# Create new utility module  
./scripts/create_module.sh create DatabaseManager

# Remove modules safely (with confirmation)
./scripts/create_endpoint.sh remove EndpointUser
```

#### Generated Structure

All scripts create complete modular structure:
- Header files in `include/ModuleName/`
- Implementation in `src/ModuleName/`
- Test suite in `tests/ModuleNameTest/`
- Flexible Makefile configuration
- Auto-registration verification tests

---

## ➕ Development Workflow

### Adding New Modules

**Use generation scripts for consistency:**

```bash
# Create endpoint (handles HTTP requests)
./scripts/create_endpoint.sh create EndpointUser

# Create service (business logic with IoC)
./scripts/create_service.sh create ServiceAuth  

# Create utility module (infrastructure/utilities)
./scripts/create_module.sh create DatabaseManager
```

**Manual development workflow:**

1. **Test-Driven Development (TDD)**:
   - Write failing tests first in `tests/ModuleNameTest/cases/`
   - Run tests: `make test-run-ModuleNameTest`
   - Implement minimal code to pass tests
   - Refactor and repeat

2. **Module Structure**:
   - Headers: `include/ModuleName/ModuleName.h`
   - Implementation: `src/ModuleName/ModuleName.cpp`
   - Tests: `tests/ModuleNameTest/cases/*.cpp`

3. **Testing Integration**:
   - Use flexible Makefile dependency configuration
   - Update `DEPENDENCIES` line in test Makefile
   - Verify with `make test-debug-ModuleNameTest`

### Adding Source Files

**Main project:**
- Add `.cpp` files to `src/ModuleName/`
- Add headers to `include/ModuleName/`
- Follow namespace conventions

**External libraries:**
- Create `external/<libname>/`
- Add sources to `external/<libname>/src/` (`.cpp` for C++, `.c` for C)
- Add headers to `external/<libname>/include/`
- Auto-detected by build system

### Code Standards

- **Microsoft C++ Style**: PascalCase classes, camelCase variables
- **Namespace per module**: `namespace modulename { ... }`
- **One class per file**: `ModuleName.h` and `ModuleName.cpp`
- **Dependency injection**: Pass dependencies explicitly, avoid global state
- **Modern C++**: Smart pointers, const correctness, explicit types

### Debugging Workflow

```bash
# Build and test cycle
make clean && make debug && make test-run-ModuleNameTest

# Quick debug run
make run_debug

# Check build configuration
make test-debug-ModuleNameTest

# Full validation
./scripts/test.sh
```

---

---

## 📦 External & Third-Party Libraries

### Source-Based External Libraries

Place source-based libraries in `external/<libname>/` with this structure:

```
external/
└── mongoose/
    ├── include/
    │   └── mongoose.h
    └── src/
        └── mongoose.c
```

**Features:**
- **Auto-detection**: Sources automatically included in build
- **Mixed languages**: C files use `gcc`, C++ files use `g++`
- **No configuration**: No Makefile changes needed
- **C++ compatibility**: Use `extern "C"` wrappers for C headers

### Pre-Built Libraries

For pre-compiled libraries:
- Place binaries (`.a`, `.so`) in `lib/`
- Place headers in `lib/include/<libname>/`
- Configure linking in `Project.build`:

```makefile
# Project.build configuration
LIB_DIRS += -L/usr/local/lib -Llib/
LDLIBS += -lmylibrary -lpthread
ADDITIONAL_INCLUDES += -Ilib/include
```

### C/C++ Interoperability

**C header with C++ compatibility:**

```c
#ifndef FOO_C_H
#define FOO_C_H

#ifdef __cplusplus
extern "C" {
#endif

void external_print_c(const char* message);

#ifdef __cplusplus
}
#endif

#endif // FOO_C_H
```

**C++ header:**

```cpp
#pragma once
void external_print(const char* message);
```

**Usage in C++:**

```cpp
#include "foo.h"
#include "foo_c.h"

int main() {
    external_print("Hello from C++!");
    external_print_c("Hello from C!");
    return 0;
}
```

---

## 🏗️ Advanced Build System

### Immutable Build System Design

The build system uses an **immutable Makefile** with configuration-driven customization:

- **Core Principle**: Main `Makefile` never modified for project-specific needs
- **Configuration Files**: All customizations in `Project.build` and `Platform.build`
- **Generic Patterns**: Build logic separated from project configuration
- **Maintainability**: Stable core with flexible configuration overlay

### Build Configuration

**Project.build** - Project-specific configuration:
```makefile
# Custom source patterns
ADDITIONAL_SOURCES += $(wildcard external/*/src/*.cpp)
ADDITIONAL_SOURCES += $(wildcard external/*/src/*.c)

# Include directories
ADDITIONAL_INCLUDES += -Iexternal/mongoose/include

# External libraries
LIB_DIRS += -L/usr/local/lib
LDLIBS += -lsqlite3 -lcurl
```

**Platform.build** - Cross-compilation and platform detection:
```makefile
# Cross-compilation support
CROSS_COMPILE_PREFIX ?= aarch64-linux-gnu-
CC = $(CROSS_COMPILE_PREFIX)gcc
CXX = $(CROSS_COMPILE_PREFIX)g++

# Architecture detection
ARCH := $(shell uname -m)
VENDOR := native
```

### Dual-Mode Output System

**Human Mode (VERBOSE=1, default)**:
- Colorful, descriptive output with emojis
- Full compilation commands visible
- Comprehensive progress indicators
- Detailed error messages

**Agent Mode (VERBOSE=0)**:
- Ultra-concise output for AI processing
- Silent compilation with error visibility
- Standardized prefixes: `[BUILD]`, `[TEST]`, `[SCRIPT]`
- Essential milestones only

```bash
# Human-friendly build
make debug
# Output: 🔨 Compiling (debug): src/main.cpp
#         🔗 Linking debug executable...

# Agent-friendly build
VERBOSE=0 make debug
# Output: [BUILD] LINK debug hello_world
#         [BUILD] BUILT debug hello_world x86_64-native
```

### Cross-Platform Support

**Docker Integration**:
```bash
# Docker cross-compilation
docker run --rm -v $(pwd):/workspace \
  -e DOCKER_CROSS_COMPILE=aarch64-linux-gnu- \
  cross-compiler make

# Multi-platform builds
docker buildx build --platform linux/arm64,linux/amd64 .
```

**Supported Architectures**:
- `x86_64`: Intel/AMD 64-bit
- `aarch64`: ARM 64-bit (Apple Silicon, modern ARM)
- `arm`: ARM 32-bit (embedded systems)

### Build Output Structure

```
bin/
└── x86_64-native/           # Architecture-vendor identifier
    ├── debug/
    │   ├── src/             # Debug object files
    │   ├── external/        # External library objects
    │   │   └── mongoose/
    │   └── hello_world      # Debug executable
    └── release/
        └── hello_world      # Release executable
```

### Build Features

- **Incremental Building**: Dependency tracking with `.d` files
- **Parallel Compilation**: Multithreaded builds with `make -j$(nproc)`
- **Mixed Languages**: Automatic C/C++ source detection
- **Clean Separation**: Objects in `bin/`, never in source directories
- **Debug/Release**: Separate configurations with appropriate flags

---

---

## � VS Code Integration

### Comprehensive Task System

The project includes extensive VS Code task integration with multithreaded builds and dual-mode output:

**Build Tasks:**
- **Build Debug** (F1 → Tasks: Run Task): `make -j$(nproc) debug`
- **Build Release**: `make -j$(nproc) release`
- **Build Debug (Silent)**: `VERBOSE=0 make -j$(nproc) debug`
- **Clean All**: `make clean`

**Run Tasks:**
- **Run Debug**: Build and execute debug version
- **Run Release**: Build and execute release version

**Test Management:**
- **Run All Tests**: Execute complete test suite with parallel builds
- **Run All Tests (Silent)**: AI-agent optimized test execution
- **Individual Module Tests**: Test specific modules (Logger, WebServer, ApiModule, etc.)
- **Test Help**: Display all available test targets
- **Test Stats**: Show project statistics and metrics

**Module Generation:**
- **Create Endpoint**: Interactive endpoint generation with input prompts
- **Create Service**: Interactive service generation with input prompts
- **Create Module**: Interactive utility module generation with input prompts

**Automation:**
- **Comprehensive Test Script**: Full project validation
- **Comprehensive Test Script (Agent Mode)**: Silent automation for CI/CD

### Advanced Debugging Configurations

**Main Application Debugging:**
- **(gdb) Launch Debug**: Standard debugging with automatic build
- **(gdb) Launch Release**: Debug optimized release builds
- **(gdb) Launch Debug with Args**: Interactive argument input
- **(gdb) Launch Debug - Stop at Entry**: Pause at program start
- **(gdb) Quick Debug - No Build**: Skip build for rapid iterations

**Test Module Debugging:**
- Individual debug configurations for every test module:
  - Logger, WebServer, ApiModule, IoCContainer
  - EndpointHello, ServiceA, ServiceB, PrintHello

**Advanced Analysis:**
- **(gdb) Debug with Valgrind**: Memory leak detection and analysis
- **(gdb) Attach to Process**: Debug running processes

### Key Features

- **Multithreaded Builds**: All tasks use `make -j$(nproc)` for maximum performance
- **Dual-Mode Output**: Human-friendly vs AI-agent optimized presentation
- **Problem Matchers**: Integrated GCC error detection and navigation
- **Interactive Inputs**: Prompted module creation with validation
- **Pre-Launch Tasks**: Automatic builds before debugging
- **Memory Analysis**: Built-in Valgrind integration for professional debugging

### Usage

**Access via Command Palette:**
1. Press `Ctrl+Shift+P` (or `Cmd+Shift+P` on macOS)
2. Type "Tasks: Run Task"
3. Select from comprehensive task list

**Quick Debugging:**
1. Press `F5` for default debug configuration
2. Or select specific debug configuration from Run and Debug panel
3. Automatic build and launch with GDB integration

**Module Generation:**
1. Run "Create Endpoint", "Create Service", or "Create Module" tasks
2. Follow interactive prompts for naming and configuration
3. Generated modules include complete structure and tests

---

## 🧪 Unit Testing

This project uses [Google Test](https://github.com/google/googletest) for C++ unit testing. Unit tests are organized by module in the `tests/` directory, with each module having its own subfolder, object files in `obj/`, and test executables in `bin/`.

**Example test structure:**
```
tests/
  printHello/
    Makefile
    printHelloTest.cpp
    test_main.cpp
    obj/
      printHelloTest.o
      test_main.o
      printHello.o
    bin/
      printHelloTest
  logger/
    ...
  ...
```

### Build and Run All Tests

From the `tests/` directory:

```sh
make test
```

This will build and run all unit tests in all subfolders. Each test module's executable is placed in its own `bin/` directory.

### Add a New Test Module
1. Create a new subfolder in `tests/` (e.g., `logger/`).
2. Add your test source files and a `Makefile` (copy from `printHello/` as a template).
3. Add the subfolder name to the `SUBDIRS` variable in `tests/Makefile`.
4. Implement your tests using Google Test.

---

## 📚 Documentation

### API Documentation

- **Modular Architecture**: Each module documented in its respective directory
- **Interface Documentation**: Clear public API contracts with usage examples
- **Code Comments**: Comprehensive inline documentation for complex logic

### Development Guidelines

- **Test-Driven Development**: Write tests first, implement minimal code to pass
- **Dependency Injection**: Explicit dependencies, no global state
- **Modern C++ Practices**: Smart pointers, const correctness, RAII principles
- **Microsoft C++ Style**: Consistent naming and formatting conventions

### Build System Documentation

- **Immutable Design**: Core Makefile never modified, configuration through `.build` files
- **Dual-Mode Output**: Human-friendly and AI-agent-optimized build output
- **Cross-Platform**: Docker integration and multi-architecture support

---

## 🤝 Contributing

### Development Workflow

1. **Use Generation Scripts**: Create new modules with provided scripts
2. **Follow TDD**: Write tests first, implement incrementally
3. **Test Integration**: Verify IoC and API module registration
4. **Code Standards**: Follow Microsoft C++ guidelines
5. **Documentation**: Update relevant documentation

### Project Conventions

- **Modular Structure**: Each feature as independent module
- **Test Organization**: Cases in `cases/` subdirectories
- **Build Configuration**: Use flexible Makefile templates
- **Automation**: Leverage provided scripts and dual-mode output

### Getting Help

- **Test Management**: `make test-help` for comprehensive testing options
- **Build Debugging**: `make test-debug-ModuleName` for dependency resolution
- **Project Statistics**: `make test-stats` for overview metrics
- **Error Resolution**: Intelligent error messages with suggestions

---

## 📄 License

This project is open source. See individual source files for specific licensing information.

---

## 🔄 Version History

**Current**: Sophisticated modular architecture with comprehensive testing framework, dual-mode build system, and automation scripts.

- ✅ Immutable build system with configuration-driven customization
- ✅ Dual-mode output (human-friendly and AI-agent-optimized)
- ✅ Comprehensive test framework with Google Test/Mock integration
- ✅ Modular architecture with IoC container and dependency injection
- ✅ WebServer integration with Mongoose backend
- ✅ Cross-platform support with Docker integration
- ✅ Automation scripts for module generation and testing
- ✅ Flexible Makefile system with dependency management

---

**This sophisticated C++ boilerplate provides a production-ready foundation for modern C++ projects with comprehensive testing, automation, and cross-platform support.**
