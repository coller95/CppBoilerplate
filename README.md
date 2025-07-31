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

## Example Code

- `main.cpp` calls the `printHello()` function.
- `printHello.cpp` implements the function, printing a message to the console.
- `printHello.h` declares the function.

---

Feel free to use this as a starting point for your own C++ projects!
