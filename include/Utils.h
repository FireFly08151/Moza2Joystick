#pragma once

#include <cstdint>
#include <string>

namespace Utils {

    struct MozaState {
        int16_t wheel;
        uint8_t throttle;
        uint8_t brake;
        uint8_t clutch;
        bool buttons[128];  // adjust if you need more buttons
    };

    long mapToVJoyAxis(int32_t value, int32_t inMin, int32_t inMax);

    // Optional overload for unsigned ranges like 0..255
    inline long mapToVJoyAxis(uint8_t value) {
        return static_cast<int32_t>(value * 128 - 32768); // 0..255 → -32768..0
    }

    std::string wstringToUtf8(const std::wstring& wstr);
}
