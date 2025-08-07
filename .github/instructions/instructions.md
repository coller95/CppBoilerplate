# Project Coding & Architectural Guidelines (Custom)

## Application Entrypoint
- The `Application` class is the **true entrypoint** for all business logic, orchestration, and dependency management.
- The `main()` function must only:
  - Instantiate the `Application` class and call its `run()` method.
  - Contain no business logic, orchestration, or dependency setup.
- All dependencies must be instantiated and managed **inside the `Application` class** (not injected from `main()`).
- All startup, shutdown, orchestration, and dependency wiring logic must reside in `Application`.

## Dependency Management
- The `Application` class is responsible for constructing all dependencies it needs (e.g., loggers, services, modules).
- Use interfaces (pure abstract classes) for dependencies to enable mocking and testability.
- No dependency injection from `main()`.

## File/Folder Naming
- All source, header, and test files and folders must use PascalCase to match class/struct names and Microsoft C++ guidelines.
- Each class or struct must be defined in its own header/source file pair (PascalCase).

## OOP & Testability
- All logic must be encapsulated in classes/structs.
- No global state or free functions for business logic.
- All logic must be testable via public interfaces.
- Favor clear interfaces for all modules.

## main() Function
- Only responsible for launching the `Application` class.
- No business logic, orchestration, or dependency setup allowed in `main()`.

## Example main.cpp Structure
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
