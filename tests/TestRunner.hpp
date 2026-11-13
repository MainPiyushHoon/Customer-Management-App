#pragma once
#include <iostream>
#include <string>
#include <vector>

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

    inline int summary() {
        std::cout << "\n=====================================\n";
        std::cout << "TEST SUMMARY: " << passed << " passed, " << failed << " failed.\n";
        std::cout << "=====================================\n";
        return failed == 0 ? 0 : 1;
    }
}
