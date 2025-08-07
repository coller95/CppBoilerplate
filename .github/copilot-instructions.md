

# AI Coding Instructions for CppBoilerplate

This project uses a **modular monolithic C++ architecture** with strict Microsoft naming conventions and clear separation of concerns.

## Architecture

- **Modular:** Each component (e.g., `Logger`, `PrintHello`) has its own files in `include/` (headers) and `src/` (implementation).
- **Single Binary:** All modules build into one executable (`hello_world`).
- **External Libraries:** Place third-party code in `external/`; auto-detected by the build system.
- **Interfaces:** Public headers define contracts; implementations are in source files.

## Naming Conventions

- **Files/Folders:** Use PascalCase for all source, header, and test files/folders (e.g., `RestfulServer.cpp`, `LoggerTest/`).
- **Classes/Structs:** PascalCase (e.g., `RestfulServerTest`)

void openFile(std::string_view fileName) {
    if (fileName.empty()) {
        throw std::invalid_argument("File name cannot be empty");
    }
    // file operations...
}

// Good: Container usage
std::vector<int> numbers{1, 2, 3, 4, 5};  // Better than int numbers[5]
```

## Modern C++ Features
- **Use `auto`** when type is obvious from context
- **Prefer smart pointers** over raw pointers (`std::unique_ptr`, `std::shared_ptr`)
- **Use range-based for loops** when iterating over containers
- **Use `const` and `constexpr`** where applicable
- **Prefer `std::string_view`** for read-only string parameters
- **Use uniform initialization** with braces `{}`
- **Avoid C-style casts**, use C++ static_cast, dynamic_cast, etc.
- **Use lambda expressions** for short, local functions
- **Prefer algorithms** from `<algorithm>` over manual loops
- **Use `nullptr`** instead of `NULL` or `0`
- **Use `using` aliases** instead of `typedef`
- **Embrace RAII** (Resource Acquisition Is Initialization)

### Examples:
```cpp
// Good: Modern C++ practices
auto myVector = std::vector<int>{1, 2, 3, 4, 5};
auto uniquePtr = std::make_unique<MyClass>(42);
constexpr auto Pi = 3.14159;

void processStrings(std::string_view input) {  // Better than const std::string&
    for (const auto& item : myVector) {  // Range-based for
        std::cout << item << " ";
    }
}

using IntVector = std::vector<int>;  // Better than typedef

// Good: Lambda for simple operations
auto isEven = [](int n) { return n % 2 == 0; };
std::count_if(myVector.begin(), myVector.end(), isEven);

// Good: Algorithm usage
std::sort(myVector.begin(), myVector.end());
```

## Example Code Structure
```cpp
#pragma once

#include <string>
#include <vector>

/**
 * Example class demonstrating the coding style
 */
class ExampleClass {
private:
    int _memberVariable;
    std::string _memberString;

public:
    ExampleClass(int initialValue, const std::string& initialString);
    
    /**
     * Gets the current value
     * @return The current integer value
     */

    int getValue() const;
    /**
     * Sets a new value with validation
     * @param newValue The new value to set
     * @return true if successful, false otherwise
     */
    bool setValue(int newValue);
    /**
     * Processes data using modern C++ features
     * @param inputData Vector of data to process
     * @return Processed results
     */
    std::vector<int> processData(const std::vector<int>& inputData) const;
};
```

## Guidelines Summary
1. Use camelCase for variables/functions
2. Use PascalCase for classes/structs/constants
3. Be consistent with naming
4. Use clear, descriptive names
5. Follow spacing/formatting rules
6. Document public interfaces
7. Use modern C++ features
8. Maintain codebase consistency
9. Prefer `constexpr` over `const`
10. Use `enum class` for type safety
11. Apply RAII for resource management
12. Prefer standard algorithms over manual loops
13. Use smart pointers for dynamic memory
14. Embrace `auto` for obvious types

## Performance Considerations
- Pass large objects by const reference or `std::string_view`
- Use move semantics (`std::move`) for ownership transfer
- Prefer `emplace_back()` over `push_back()`
- Use `reserve()` for vectors when size is known
- Avoid unnecessary copies (perfect forwarding)
- Use `noexcept` for functions that don't throw

### Examples:
```cpp
void processData(std::string_view data) noexcept {
    std::vector<int> results;
    results.reserve(data.size());
    for (char c : data) {
        results.emplace_back(static_cast<int>(c));
    }
}
class DataProcessor {
private:
    std::vector<std::string> _data;
public:
    void addData(std::string data) {
        _data.emplace_back(std::move(data));
    }
};
```


These guidelines ensure code maintainability, readability, and consistency across the project.

---

**Note:** When referring to actions on "the whole repo," always exclude files/folders listed in `.gitignore`.

## Efficient Debug & Test Workflow

1. **Always work from the project root.**
2. **Batch build & test:** Use batch commands to build and test in one step for rapid feedback.
   - Main app: `make debug && make run_debug`
   - All unit tests: `make test`
   - Clean & build (if needed):
     - `make clean && make debug && make run_debug`
     - `make test-clean && make test`
3. **After every change:** Run relevant unit tests to catch issues early.
4. **Iterative debugging:** If a test fails, fix and rerun up to 5 times, reviewing all output.
5. **Escalate if needed:** If problems persist after 5 attempts, escalate with detailed logs and context.

---
---

# Feature Development Workflow (CppBoilerplate)

This document describes the standard workflow for developing a new feature in this C++ project. All contributors must follow these steps to ensure code quality, modularity, and testability.

## 1. Define the Feature
- Clearly describe the feature’s purpose and requirements.

## 2. Design the Module
- Choose a descriptive PascalCase name for the new class/module.
- Plan the public interface (methods, data members) and dependencies (use dependency injection).

## 3. Create Module Files
- Add a header file in `include/` (e.g., `NewFeature.h`).
- Add a source file in `src/` (e.g., `NewFeature.cpp`).
- Interface in header, implementation in source, one class per file.

## 4. Write Unit Tests First (TDD)
- Create a test folder: `tests/newFeature/`.
- Add test files (e.g., `newFeatureTest.cpp`, `test_main.cpp`).
- Write Google Test cases covering the new feature’s public API.
- Place all test/mocking code only in the test folder.

## 5. Implement the Feature
- Write the class implementation in `src/NewFeature.cpp`.
- Use modern C++ practices, smart pointers, and follow naming/formatting rules.

## 6. Build and Test
- From the project root, always run:
  ```bash
  make test-clean
  make debug
  make test
  ```
- This will clean all test artifacts, build the debug version, and run all unit tests (no need to cd into tests/).

## 7. Document
- Add comments to public interfaces and complex logic.

## Feature Development Workflow
1. Define the feature’s purpose and requirements.
2. Design the module (PascalCase name, public interface, dependencies).
3. Create module files: header in `include/`, source in `src/` (PascalCase, one class per file).
4. Write unit tests first (TDD): `tests/ModuleNameTest/` (PascalCase folder).
5. Implement the feature using modern C++ and project style.
6. Build and test: always run `make test-clean`, `make debug`, `make test` from project root.
7. Document public interfaces and complex logic.
8. Integrate: connect to main app if needed, encapsulate all logic in classes.
9. Review and refactor for style, modularity, and test coverage.

This workflow ensures all code is modular, testable, and maintainable.

---

# Project Coding & Architectural Guidelines

## Application Entrypoint
- `Application` is the entrypoint for all business logic and dependency management.
- `main()` only instantiates `Application` and calls `run()`—no business logic or dependency setup.
- All dependencies are constructed and managed inside `Application`.

## Dependency Management
- `Application` constructs all dependencies (loggers, services, modules).
- Use interfaces for dependencies to enable mocking and testability.

## File/Folder Naming
- Use PascalCase for all source, header, and test files/folders.
- One class/struct per header/source file pair (PascalCase).

## OOP & Testability
- Encapsulate all logic in classes/structs.
- No global state or free functions for business logic.
- All logic must be testable via public interfaces.
- Favor clear interfaces for all modules.

## main() Function
- Only launches `Application` (no business logic or dependency setup).
  ```cpp
  #include <Application/Application.h>
  int main() {
      Application app;
      return app.run();
  }
  ```

## Summary
- `Application` is the entrypoint for all program logic and dependency management.
- `main()` is only for launching `Application`.
- All dependencies are constructed inside `Application`.
- Use PascalCase for all files, folders, and types.
- No business logic or dependency setup in `main()`.


---
applyTo: '**'
---
Provide project context and coding guidelines that AI should follow when generating code, answering questions, or reviewing changes.



## Unit Test Best Practices & TDD

- **Practice TDD:** Write unit tests before or alongside production code. For every new feature or bugfix, add or update tests describing expected behavior.
- **Testable Design:** Design code for easy unit testing and minimal dependencies.
- **Dependency Injection:** Favor dependency injection and clear interfaces for all modules to enable mocking and isolation in tests.
- **No Global State:** Avoid global state and side effects; always pass dependencies explicitly.
- **Test Organization:**
  - For every new module/feature, add or update unit tests in `tests/ModuleNameTest/` (PascalCase folder).
  - Place all test-only code (mocks, helpers) inside the module's test folder, never in production `src/` or `include/`.
- **Test Makefile Structure:**
  - All new test Makefiles must follow the pattern of `tests/PrintHelloTest/Makefile`:
    - Use `obj/` for object files and `bin/` for test executables.
    - Use variables for sources, objects, and binaries.
    - Compile all test and implementation sources to `obj/*.o`.
    - Link against prebuilt Google Test libraries.
    - Provide explicit rules for compiling sources and creating directories.
    - Example:
      ```makefile
      OBJDIR = obj
      BINDIR = bin
      TEST_SRC = MyTest.cpp TestMain.cpp ../../src/MyModule/MyModule.cpp
      TEST_OBJS = $(OBJDIR)/MyTest.o $(OBJDIR)/TestMain.o $(OBJDIR)/MyModule.o
      TEST_BIN = $(BINDIR)/MyTest

      all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

      $(OBJDIR):
        mkdir -p $(OBJDIR)

      $(BINDIR):
        mkdir -p $(BINDIR)

      $(OBJDIR)/%.o: %.cpp | $(OBJDIR)
        $(CXX) $(CXXFLAGS) -c $< -o $@

      $(OBJDIR)/MyModule.o: ../../src/MyModule/MyModule.cpp | $(OBJDIR)
        $(CXX) $(CXXFLAGS) -c $< -o $@

      $(TEST_BIN): $(TEST_OBJS)
        $(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

      clean:
        rm -f $(OBJDIR)/*.o $(BINDIR)/MyTest
      ```
- **Modular Monolithic Architecture:** Each module should have its own test suite, with object files in `obj/` and test executables in `bin/`.
- **Test Coverage:** All public APIs and logic should be covered by tests; avoid tightly coupling code to I/O or system state.
- **Google Test:** Use Google Test for all C++ unit tests, and ensure all code is built with debug symbols (`-g`).
- **Test Isolation:** Use mocks and dependency injection to isolate tests from real system/network state. Avoid tests that depend on external resources unless explicitly required.
- **Naming & Documentation:** Name test cases and mocks clearly. Add comments to clarify the purpose of each test and mock.
- **Continuous Improvement:** Refactor tests and production code together to maintain clarity, coverage, and maintainability.