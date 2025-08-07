
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

2. **Write a Failing Test**
   - Create a new test in `tests/ModuleNameTest/`.
   - Use Google Test; name test files and cases in PascalCase.
   - Focus on the public interface and expected behavior.

   - For every new module/feature, add or update unit tests in `tests/ModuleNameTest/` (PascalCase folder).
   - Place all test-only code (mocks, helpers) inside the module's test folder, never in production `src/` or `include/`.

3. **Build and Run Tests**
   - Run `make test-clean`, `make debug`, `make test` from the project root.
   - Confirm the new test fails (red).

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

4. **Implement the Minimal Code**
   - Write just enough code in `src/ModuleName/ModuleName.cpp` to make the test pass.
   - Follow modern C++ and project style guidelines.

   - All public APIs and logic should be covered by tests; avoid tightly coupling code to I/O or system state.

5. **Rebuild and Retest**
   - Run the test suite again.
   - Ensure the new test passes (green) and no regressions occur.

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