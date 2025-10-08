#pragma once

#include <cstdint>

namespace Utils {
    long mapToVJoyAxis(int32_t value, int32_t inMin, int32_t inMax);

    // Optional overload for unsigned ranges like 0..255
    inline long mapToVJoyAxis(uint8_t value) {
        return static_cast<int32_t>(value * 128 - 32768); // 0..255 → -32768..0
    }
}
