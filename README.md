# CppBoilerplate

A minimal C++ project boilerplate with a simple build system using Makefiles. This project demonstrates a basic structure for C++ projects, including source files, headers, and support for third-party libraries.

## Project Structure

```
CppBoilerplate/
├── Makefile            # Main build file
├── Makefile.build      # Build configuration and variables
├── include/            # Project header files
│   └── printHello.h    # Example header
├── lib/                # Third-party libraries and headers
│   └── include/        # Third-party headers (empty)
├── src/                # Source files
│   ├── main.cpp        # Entry point
│   └── printHello.cpp  # Example implementation
```

## Building the Project

To build the project, simply run:

```bash
make
```

This will build the release target by default. For a debug build, run:

```bash
make debug
```

The binaries will be placed in the `bin/` directory, organized by architecture and build type.

## Usage

After building, run the executable (replace `<arch-vendor>` with your detected architecture and vendor, e.g., `x86_64-native`):

```bash
./bin/<arch-vendor>/release/hello_world
```

## Adding Source Files

- Place new `.cpp` files in the `src/` directory.
- Place new headers in the `include/` directory.

## Third-Party Libraries

- Place library files (`.a` or `.so`) in the `lib/` directory.
- Place third-party headers in `lib/include/<libname>/`.
- Update the `Makefile` to link against new libraries as needed.


## Debugging with VS Code

This project includes pre-configured VS Code settings for building and debugging:

- `.vscode/launch.json` for debugging with GDB
- `.vscode/tasks.json` for building with `make debug`

### How to Debug

1. Open this project in VS Code.
2. Press `F5` or go to the Run & Debug panel and select **(gdb) Launch Debug**.
3. The project will be built in debug mode and the debugger will start.
4. You can set breakpoints, step through code, and inspect variables.

The debug binary is expected at:

```bash
./bin/x86_64-native/debug/hello_world
```

If your architecture is different, adjust the path in `.vscode/launch.json` accordingly.

---

Feel free to use this as a starting point for your own C++ projects!
