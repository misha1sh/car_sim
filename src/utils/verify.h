#pragma once

#include <stdexcept>

#define VERIFY(condition) { \
    if (condition) {        \
    } else {       \
        std::cerr << "FAILED CHECK: " << #condition << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        throw std::logic_error("FAILED CHECK: " #condition);\
    }                           \
}


