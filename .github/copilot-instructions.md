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

---

## Coding Standards

### Microsoft C++ Coding Style

- **Naming Conventions:**
  - Use PascalCase for class names, structs, enums, and namespaces
  - Use camelCase for variable names, function parameters, and local functions
  - Use ALL_CAPS for macros and compile-time constants
  - One file per class: each class should have its own header (.h) and source (.cpp) file, named after the class

- **Code Formatting:**
  - Indent with 4 spaces, no tabs
  - Place opening braces on the same line as the declaration (K&R style)
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

### Module Generation Script

- Use the `scripts/create_module.sh` script to generate new utility/infrastructure modules automatically
- Usage:
  - Create: `./scripts/create_module.sh create ModuleName` (must be in PascalCase, cannot start with "Service" or "Endpoint")
  - Remove: `./scripts/create_module.sh remove ModuleName` (removes an existing module with confirmation)
- The script creates the complete modular structure with interface, implementation, and comprehensive tests:
  - `include/ModuleName/IModuleName.h` (interface definition)
  - `include/ModuleName/ModuleName.h` (main implementation header)
  - `src/ModuleName/ModuleName.cpp` (implementation)
  - `tests/ModuleNameTest/` (with comprehensive test cases and mocks)
- Generated modules follow RAII principles and include mock classes for testing
- Always test the generated module with `make test-run-ModuleNameTest`
- Examples: `DatabaseManager`, `ConfigReader`, `CacheManager`, `FileHandler`

**Rationale:** Infrastructure modules require different patterns than services/endpoints, including interfaces for testability, RAII resource management, and mock objects for testing dependent components.

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
  - The endpoint module is automatically registered with ApiModules
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

## Module Generation Script

- Use the `scripts/create_module.sh` script to generate new utility/infrastructure modules automatically
- Usage:
  - Create: `./scripts/create_module.sh create ModuleName` (must be in PascalCase, cannot start with "Service" or "Endpoint")
  - Remove: `./scripts/create_module.sh remove ModuleName` (removes an existing module with confirmation)
- The script creates the complete modular structure with interface, implementation, and comprehensive tests:
  - `include/ModuleName/IModuleName.h` (interface definition)
  - `include/ModuleName/ModuleName.h` (main implementation header)
  - `src/ModuleName/ModuleName.cpp` (implementation)
  - `tests/ModuleNameTest/` (with comprehensive test cases and mocks)
- Generated modules follow RAII principles and include mock classes for testing
- Always test the generated module with `make test-run-ModuleNameTest`
- Examples: `DatabaseManager`, `ConfigReader`, `CacheManager`, `FileHandler`

**Rationale:** Infrastructure modules require different patterns than services/endpoints, including interfaces for testability, RAII resource management, and mock objects for testing dependent components.

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


- **Example Pattern:**
    ```makefile


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
- Indent with 4 spaces, no tabs.
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
- Special folder structures use the format: `ModuleName:FolderName` (e.g., `ApiModules:ApiModule`)

#### Key Features
- **Easy dependency management**: Just add module names to the `DEPENDENCIES` variable
- **Automatic compilation rules**: Generic patterns handle any module type (C++17 standard)
- **Debug support**: Use `make debug-config` to verify dependency resolution
- **External library support**: Use `EXTERNAL_DEPS` for dependencies from `external/` folder
- **Enhanced testing**: Includes both Google Test and Google Mock support
- **Consistent structure**: All Makefiles follow the same template

#### Usage Examples

**Simple service with IoC dependency:**
```makefile
MODULE_NAME = ServiceA
DEPENDENCIES = IoCContainer
```

**Endpoint with API modules:**
```makefile
MODULE_NAME = EndpointUser
DEPENDENCIES = ApiModules:ApiModule
```

**Complex module with multiple dependencies:**
```makefile
MODULE_NAME = DatabaseManager
DEPENDENCIES = Logger IoCContainer
EXTERNAL_DEPS = mongoose
```

#### Debugging Dependencies
Use the built-in debug target to verify your configuration:
```bash
cd tests/ModuleNameTest
make debug-config
```

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

**Rationale:** The flexible Makefile system eliminates boilerplate, reduces errors, ensures consistency across all test modules, and makes dependency management trivial for developers. The enhanced test management system provides a seamless experience for both individual module development and project-wide testing operations.

### Legacy Makefile Example (for reference)
```makefile
# Modern, robust Makefile for unit tests (Google Test)
ROOTDIR = ../..
CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -I$(ROOTDIR)/include -I$(ROOTDIR)/external/googletest/googletest/include -g -Wall -Wextra -MMD -MP
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

#### Makefile Generation Requirements

Generated test Makefiles must:
- Use the **flexible Makefile template** with configuration-based dependency management
- Set `MODULE_NAME = <ModuleName>` in the CONFIGURATION SECTION at the top
- Specify dependencies using `DEPENDENCIES = ModuleA ModuleB` (simple format) or `ModuleA:FolderA ModuleB` (complex format)
- Use `EXTERNAL_DEPS = library1 library2` for external dependencies from `external/` folder
- Include `cases/*.cpp` glob for test cases and keep the test runner at the module root
- Place all objects in `obj/` and the binary in `bin/` inside the test module folder
- **Use real tabs for all recipe lines** (critical for make compatibility)
- Support both Google Test and Google Mock for comprehensive testing capabilities

#### Heredoc Best Practices

- **Always use single-quoted heredoc for Makefiles:** `cat << 'EOF'` to preserve Makefile variables (`$(...)`)
- This prevents bash from expanding `$` characters, ensuring Makefile variables remain intact
- After generating the Makefile, use `sed` to replace placeholders: `sed -i "s/ModuleName/$ACTUAL_NAME/g"`
- **Never mix bash variable expansion with Makefile variables in the same heredoc**

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

- Strict adherence to TDD is mandatory for all development tasks
- Always use the generation scripts for creating new modules:
  - `./scripts/create_endpoint.sh` for endpoints 
  - `./scripts/create_service.sh` for services
  - `./scripts/create_module.sh` for utility/infrastructure modules
- Use the flexible Makefile system: modify only the DEPENDENCIES line in test Makefiles
- Use `make debug-config` to verify dependency resolution in test modules
- Follow the modular monolithic architecture principles
- Ensure all Makefiles use real tabs for recipe lines
- Place test cases in `cases/` subfolders for scalability
- Use dependency injection and clear interfaces for testability
- Run relevant tests after every significant change

---

*Refer to the [Microsoft C++ Coding Guidelines](https://learn.microsoft.com/en-us/cpp/cpp/cpp-coding-guidelines) for additional details on coding standards.*