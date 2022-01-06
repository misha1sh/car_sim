#pragma once

namespace math_utils {
    template <typename T>
    inline T Crop(const T& value, const T& min_val, const T& max_val) {
        if (value < min_val) {
            return min_val;
        }
        if (value > max_val) {
            return max_val;
        }
        return value;
    }
}