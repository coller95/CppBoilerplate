---
applyTo: '**'
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
- Update README or module documentation if needed.

## 8. Integrate
- Connect the new feature to the main application if required.
- Ensure all logic is encapsulated in classes (no global state).

## 9. Review and Refactor
- Ensure code style, modularity, and test coverage.
- Refactor for clarity and maintainability.

---

This workflow ensures all code is modular, testable, and easy to maintain. Follow this process for every new feature or significant change.
