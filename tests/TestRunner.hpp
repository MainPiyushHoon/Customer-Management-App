#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

namespace TestRunner {
    inline int passed = 0;
    inline int failed = 0;

    inline void assert_true(bool condition, const std::string& message) {
        if (condition) {
            std::cout << "  [PASS] " << message << "\n";
            passed++;
        } else {
            std::cerr << "  [FAIL] " << message << "\n";
            failed++;
        }
    }

    inline void assert_near(double actual, double expected, const std::string& message, double eps = 1e-4) {
        if (std::abs(actual - expected) <= eps) {
            std::cout << "  [PASS] " << message << "\n";
            passed++;
        } else {
            std::cerr << "  [FAIL] " << message << " (expected: " << expected << ", actual: " << actual << ")\n";
            failed++;
        }
    }

    inline int summary() {
        std::cout << "\n=====================================\n";
        std::cout << "TEST SUMMARY: " << passed << " passed, " << failed << " failed.\n";
        std::cout << "=====================================\n";
        return failed == 0 ? 0 : 1;
    }
}
