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
    # Compile C++

    # Compile C
obj/%.o: src/%.c
\tmkdir -p $(dir $@)

**Test Makefiles:**

      - Follow all main Makefile standards: always use `g++` for `.cpp` and `gcc` for `.c` files, never mix compilers for the same file type.
      - Compile all test and implementation sources to object files (`*.o`) and dependency files (`*.d`) in `obj/`, never in the source or test code directories.
      - Place all test executables in `bin/` inside the test module folder (e.g., `tests/ModuleNameTest/bin/`).
      - Never pollute the main build output with test artifacts; keep all test objects, binaries, and dependencies isolated in the test module's `obj/` and `bin/` folders.
      - Always provide a `clean` target that removes all test objects, dependencies, and binaries (e.g., `rm -rf obj/* bin/*`).
      - Use pattern rules and variables for sources, objects, and binaries to avoid duplication.
      - Use `-MMD -MP` for dependency generation and `-include obj/*.d` for incremental builds.
      - Always use `mkdir -p $(dir $@)` in every object/dependency rule to ensure all parent directories are created, especially for sources outside the test directory.
      - Document any non-obvious logic with comments.
      - Example (modern, robust, flat obj/ mapping):
        ```makefile
        # Modern, robust Makefile for unit tests (Google Test)
        # All object and dependency files are placed flat in obj/ (not nested)

        CXX = g++
        CC = gcc
        CXXFLAGS = -std=c++20 -I../../include -I../../external/googletest/googletest/include -g -Wall -Wextra -MMD -MP
        CFLAGS = -I../../include -g -Wall -Wextra -MMD -MP
        GTEST_LIBS = -L../../external/googletest/build/lib -lgtest -lgtest_main

        OBJDIR = obj
        BINDIR = bin

        TEST_SRC = MyTest.cpp TestMain.cpp ../../src/MyModule/MyModule.cpp
        # Explicit object/dependency mapping for sources outside test dir
        TEST_OBJS = $(OBJDIR)/MyTest.o $(OBJDIR)/TestMain.o $(OBJDIR)/MyModule.o
        TEST_DEPS = $(OBJDIR)/MyTest.d $(OBJDIR)/TestMain.d $(OBJDIR)/MyModule.d
        TEST_BIN = $(BINDIR)/MyTest

        all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
\tmkdir -p $(OBJDIR)

$(BINDIR):
\tmkdir -p $(BINDIR)

# Pattern rule for C++ sources in this directory
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
\tmkdir -p $(dir $@)
\t$(CXX) $(CXXFLAGS) -c $< -o $@

# Explicit rule for sources outside test dir
$(OBJDIR)/MyModule.o: ../../src/MyModule/MyModule.cpp | $(OBJDIR)
\tmkdir -p $(dir $@)
\t$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for C sources (if any)
$(OBJDIR)/%.o: %.c | $(OBJDIR)
\tmkdir -p $(dir $@)
\t$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
\t$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

-include $(TEST_DEPS)

clean:
\trm -rf $(OBJDIR)/* $(BINDIR)/*
        ```

- **General Principle:**
    - The build system should be predictable, maintainable, and easy for any developer to understand and extend.

Refer to the [Microsoft C++ Coding Guidelines](https://learn.microsoft.com/en-us/cpp/cpp/cpp-coding-guidelines) for more details.

## Microsoft C++ Coding Style

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

## Test-Driven Development (TDD) Workflow

1. **Define the Requirement**
   - Clearly state the feature or bug to address.

   - Practice TDD: Write unit tests before or alongside production code. For every new feature or bugfix, add or update tests describing expected behavior.


## Efficient Targeted Test Build & Clean

- The test build system supports running, building, or cleaning tests for a specific test directory:
    - `make test-<DirName>`: Build and run only the tests in `<DirName>` (e.g., `make test-WebServerTest`).
    - `make test-make-<DirName>`: Build (but do not run) only the tests in `<DirName>`.
    - `make test-clean-<DirName>`: Clean only the test artifacts in `<DirName>`.
- These pattern rules are available from the project root and are delegated to the corresponding subdirectory Makefiles.
- This enables rapid, focused development and cleanup for individual test modules.

2. **Write a Failing Test**
   - Create a new test in `tests/ModuleNameTest/`.
   - Use Google Test; name test files and cases in PascalCase.
   - Focus on the public interface and expected behavior.

   - For every new module/feature, add or update unit tests in `tests/ModuleNameTest/` (PascalCase folder).
   - Place all test-only code (mocks, helpers) inside the module's test folder, never in production `src/` or `include/`.


3. **Build and Run Tests (After Every Major Change!)**
   - **After every major code or build change, always run the full build and test suite to catch errors early.**
   - Run `make test-clean`, `make debug`, `make test` from the project root.
   - Confirm the new test fails (red).

   - All new test Makefiles must follow the pattern of `tests/PrintHelloTest/Makefile`:
     - Use `obj/` for object files and `bin/` for test executables.
     - Use variables for sources, objects, and binaries.
     - Compile all test and implementation sources to `obj/*.o`.
     - Link against prebuilt Google Test libraries.
     - please refer to the **Test Makefiles:** for detailed examples.

4. **Implement the Minimal Code**
   - Write just enough code in `src/ModuleName/ModuleName.cpp` to make the test pass.
   - Follow modern C++ and project style guidelines.

   - All public APIs and logic should be covered by tests; avoid tightly coupling code to I/O or system state.


5. **Rebuild and Retest (Mandatory After Each Major Change)**
   - Run the test suite again after any significant code, build, or dependency change.
   - Ensure the new test passes (green) and no regressions occur.
   - If you skip this step, you risk introducing breaking changes—**always verify with tests and debug builds!**

   - Use Google Test for all C++ unit tests, and ensure all code is built with debug symbols (`-g`).

6. **Refactor**
   - Clean up code and tests for clarity, modularity, and maintainability.
   - Use smart pointers, dependency injection, and standard algorithms.

   - Refactor tests and production code together to maintain clarity, coverage, and maintainability.

7. **Repeat**
   - For each new requirement or bug, repeat the cycle.
   - Maintain high test coverage and code quality.

   - Use mocks and dependency injection to isolate tests from real system/network state. Avoid tests that depend on external resources unless explicitly required.

8. **Document**
   - Add comments to public interfaces and complex logic.
   - Clearly document the purpose of each test.

   - Name test cases and mocks clearly. Add comments to clarify the purpose of each test and mock.

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