#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Define export macro for shared library
#ifdef _WIN32
    #ifdef MATHLIB_EXPORTS
        #define MATHLIB_API __declspec(dllexport)
    #else
        #define MATHLIB_API __declspec(dllimport)
    #endif
#else
    #define MATHLIB_API __attribute__((visibility("default")))
#endif

// Math library functions
MATHLIB_API double math_add(double a, double b);
MATHLIB_API double math_subtract(double a, double b);
MATHLIB_API double math_multiply(double a, double b);
MATHLIB_API double math_divide(double a, double b);
MATHLIB_API double math_power(double base, double exponent);

#ifdef __cplusplus
}
#endif
