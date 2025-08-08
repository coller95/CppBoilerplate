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
    # Compile C++

    # Compile C
    obj/%.o: src/%.c
        mkdir -p $(dir $@)

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



      - **Test Case Organization and Makefile Globbing (Universal Pattern)**

        - For every test module (e.g., `tests/ModuleNameTest/`), all test case `.cpp` files **must** be placed in a `cases/` subfolder (e.g., `tests/ModuleNameTest/cases/`).
        - The module's `Makefile` must use a glob pattern (`cases/*.cpp`) to automatically include all test case files in that folder as sources and objects.
        - Only the test runner (e.g., `TestMain.cpp`) should remain at the root of the test module folder.
        - To add a new test, **always** create a new `.cpp` file in the `cases/` folder—no Makefile changes required.
        - This approach keeps each test folder clean, makes test management scalable, and ensures new tests are always built.

        - Example Makefile snippet:
          ```makefile
          TEST_SRC = cases/*.cpp TestMain.cpp ../../src/ModuleName/ModuleName.cpp ...
          TEST_OBJS = $(patsubst cases/%.cpp,$(OBJDIR)/%.o,$(wildcard cases/*.cpp)) $(OBJDIR)/TestMain.o $(OBJDIR)/ModuleName.o ...
          TEST_DEPS = $(patsubst %.cpp,$(OBJDIR)/%.d,$(notdir $(basename $(wildcard cases/*.cpp)))) $(OBJDIR)/TestMain.d $(OBJDIR)/ModuleName.d ...
          ```
        - **Rationale:** This convention enables rapid test development, easy file navigation, and eliminates manual Makefile edits for each new test file in any module.
        - **Always ensure new test cases are added to the `cases/` folder to maintain consistency.**

      - Example (modern, robust, flat obj/ mapping for a test module):
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

        TEST_SRC = cases/*.cpp TestMain.cpp ../../src/ModuleName/ModuleName.cpp
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
        $(OBJDIR)/ModuleName.o: ../../src/ModuleName/ModuleName.cpp | $(OBJDIR)
            mkdir -p $(dir $@)
            $(CXX) $(CXXFLAGS) -c $< -o $@

        # Pattern rule for C sources (if any)
        $(OBJDIR)/%.o: %.c | $(OBJDIR)
            mkdir -p $(dir $@)
            $(CC) $(CFLAGS) -c $< -o $@

        $(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
            $(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

        -include $(TEST_DEPS)

        clean:
            rm -rf $(OBJDIR)/* $(BINDIR)/*
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


**Remember:** For any action requiring changes to development code, you must strictly follow TDD. Write test cases first, always. This is non-negotiable and critical to the project's success.

## Test-Driven Development (TDD) Workflow and Enforcement

### Workflow

1. **Define the Requirement**
   - Clearly state the feature or bug to address.
   - Write unit tests before or alongside production code. For every new feature or bugfix, add or update tests describing expected behavior.
   - Strictly follow the TDD workflow to ensure quality and maintainability.

2. **Write a Failing Test**
   - Clean the existing build and test artifacts before making changes:
     - Run `make clean` to remove all build artifacts.
     - Run `make test-clean` to remove all test artifacts.
   - Create a new test in `tests/ModuleNameTest/cases/`.
   - Use Google Test; name test files and cases in PascalCase.
   - Focus on the public interface and expected behavior.
   - Place all test-only code (mocks, helpers) inside the module's test folder, never in production `src/` or `include/`.

3. **Build and Run Tests**
   - After every major code or build change, always run the relevant module's tests to catch errors early:
     - Clean test artifacts: `make test-clean-<ModuleName>` (e.g., `make test-clean-WebServerTest`).
     - Build tests only: `make test-make-<ModuleName>` (e.g., `make test-make-WebServerTest`).
     - Build and run tests: `make test-run-<ModuleName>` (e.g., `make test-run-WebServerTest`).
   - Confirm the new test fails (red) before proceeding to implementation.
   - Only run the full test suite (`make test-clean`, `make debug`, `make test`) if explicitly requested.

4. **Implement the Minimal Code**
   - Write just enough code in `src/ModuleName/ModuleName.cpp` to make the test pass.
   - Follow modern C++ and project style guidelines.
   - Ensure all public APIs and logic are covered by tests; avoid tightly coupling code to I/O or system state.

5. **Rebuild and Retest**
   - Run the test suite again after any significant code, build, or dependency change.
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

### Enforcement

To ensure high-quality, maintainable code, **strict adherence to TDD** is mandatory for all development tasks. The following rules must be followed:

1. **Define the Requirement**
   - Clearly state the feature or bug to address before writing any code.

2. **Write a Failing Test First**
   - Create a new test in the appropriate `tests/ModuleNameTest/cases/` folder.
   - Ensure the test reflects the expected behavior of the feature or bugfix.
   - Confirm the test fails (red) before proceeding to implementation.

3. **Implement Minimal Code**
   - Write just enough code to make the failing test pass.
   - Avoid over-engineering or adding unnecessary functionality.

4. **Rebuild and Retest**
   - Run the test suite after every significant change.
   - Ensure all tests pass (green) and no regressions occur.

5. **Refactor**
   - Clean up code and tests for clarity, modularity, and maintainability.
   - Refactor both tests and production code together to maintain consistency.

6. **Repeat**
   - For each new requirement or bug, repeat the TDD cycle.
   - Maintain high test coverage and code quality.

### Example

1. **Define the Requirement**
   - Added a new feature to allow serving static files with a user-defined MIME type.

2. **Write a Failing Test**
   - Created a test case `ServeStaticWithMimeTest` in `tests/WebServerTest/cases/ServeStaticWithMimeTest.cpp`.
   - The test verified that the server correctly serves files with the specified MIME type.

3. **Implement Minimal Code**
   - Updated the `WebServer` interface to include the `serveStaticWithMime` method.
   - Implemented the method in `MongooseWebServerImpl` and ensured it handled MIME types.

4. **Rebuild and Retest**
   - Ran the test suite to confirm the new test failed initially.
   - Incrementally implemented the feature and re-ran tests after each change.

5. **Refactor**
   - Cleaned up the implementation to ensure clarity and maintainability.
   - Resolved warnings and ensured all tests passed without issues.

6. **Repeat**
   - Followed the TDD cycle for each incremental change, ensuring high-quality code.

### Resolving Warnings:
- During the implementation, a warning was encountered regarding the missing initializer for the `mimeType` field in the `StaticMapping` struct.
- The issue was resolved by explicitly initializing the `mimeType` field with an empty string (`""`) in the `MongooseWebServerImpl` implementation.
- This step ensured clean builds and adherence to best practices.

### Key Takeaways:
- Strict adherence to TDD ensures predictable and reliable code behavior.
- Incremental development and testing catch issues early and improve maintainability.
- Documenting the process helps maintain consistency and serves as a reference for future tasks.


### Prohibited Actions
- **No direct implementation without a failing test.**
- **No skipping of test validation after changes.**
- **No merging of code that has not been tested.**

### Rationale
Strict TDD ensures:
- Predictable and reliable code behavior.
- Early detection of bugs and regressions.
- Maintainable and scalable codebase.

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

### Rationale:
- **Efficiency**: Running only the relevant tests saves time.
- **Focus**: Limits output to the module you're working on, making debugging easier.
- **Scalability**: As the project grows, this approach ensures manageable test runs.

