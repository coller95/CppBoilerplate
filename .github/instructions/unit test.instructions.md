---
applyTo: '**'
---
Provide project context and coding guidelines that AI should follow when generating code, answering questions, or reviewing changes.


## Unit Test Best Practices & Test-Driven Development (TDD)

- **Practice TDD:** Write unit tests before or alongside production code. For every new feature or bugfix, first add or update tests that describe the expected behavior.
- **Testable Design:** Always write testable code—design functions and classes so they can be easily unit tested in isolation, with minimal dependencies.
- **Dependency Injection:** Favor dependency injection and clear interfaces for all modules to enable mocking and isolation in tests.
- **No Global State:** Avoid global state and side effects; always prefer passing dependencies explicitly.
- **Test Organization:**
  - For every new module or feature, add or update unit tests in the corresponding `tests/<ModuleName>Test/` folder (use PascalCase for folder names).
  - Place all test-only code (mocks, test helpers) inside the module's test folder, either in the test `.cpp` file or a dedicated test helper header (e.g., `Mock<ModuleName>.h`).
  - Never put test/mocking code in production `src/` or `include/` directories.
- **Test Makefile Structure:**
  - All new test Makefiles **must follow the structure of `tests/PrintHelloTest/Makefile`**:
    - Use `obj/` for object files and `bin/` for test executables (create these directories if they do not exist).
    - Use variables for source files, object files, and binary names.
    - Compile all test and implementation sources to `obj/*.o`.
    - Link against prebuilt Google Test libraries using variables for library paths.
    - Provide explicit rules for compiling sources and creating directories.
    - Example pattern:
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