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
  - For every new module or feature, add or update unit tests in the corresponding `tests/<module>/` folder.
  - Place all test-only code (mocks, test helpers) inside the module's test folder, either in the test `.cpp` file or a dedicated test helper header (e.g., `mock<Module>Deps.h`).
  - Never put test/mocking code in production `src/` or `include/` directories.
- **Modular Monolithic Architecture:** Each module should have its own test suite, with object files in `obj/` and test executables in `bin/`.
- **Test Coverage:** All public APIs and logic should be covered by tests; avoid tightly coupling code to I/O or system state.
- **Google Test:** Use Google Test for all C++ unit tests, and ensure all code is built with debug symbols (`-g`).
- **Test Isolation:** Use mocks and dependency injection to isolate tests from real system/network state. Avoid tests that depend on external resources unless explicitly required.
- **Naming & Documentation:** Name test cases and mocks clearly. Add comments to clarify the purpose of each test and mock.
- **Continuous Improvement:** Refactor tests and production code together to maintain clarity, coverage, and maintainability.