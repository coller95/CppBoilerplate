#include "mathlib.h"
#include <math.h>

double math_add(double a, double b) {
    return a + b;
}

double math_subtract(double a, double b) {
    return a - b;
}

double math_multiply(double a, double b) {
    return a * b;
}

double math_divide(double a, double b) {
    if (b == 0) {
        return 0.0; // Should handle errors better in production
    }
    return a / b;
}

double math_power(double base, double exponent) {
    return pow(base, exponent);
}
