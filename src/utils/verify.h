#pragma once

#include <stdexcept>

#define VERIFY(condition) { \
    bool CHECK_RESULT_ = condition; \
    if (!CHECK_RESULT_) {       \
        std::cerr << "FAILED CHECK: " << #condition << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        throw std::logic_error("FAILED CHECK: " #condition);\
    }                           \
}


