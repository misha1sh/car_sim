#pragma once

#include <stdexcept>

#include <iostream>
#include <csignal>
#define VERIFY(condition) { \
    if (condition) {        \
    } else {       \
        std::cerr << "FAILED CHECK: " << #condition << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        std::raise(SIGINT);\
    }                           \
}

#define FAIL(value) []() { \
    std::cerr << "FAILED CHECK: " << #value << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
    std::raise(SIGINT);                                               \
    return 0;              \
}()\

#define VERIFY_VALUE(value) value ? value : (FAIL(value), value)
// throw std::logic_error("FAILED CHECK: " #value);








