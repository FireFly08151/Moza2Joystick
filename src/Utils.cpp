#include "Utils.h"
#include "windows.h"

long Utils::mapToVJoyAxis(int32_t value, int32_t inMin, int32_t inMax) {
    if (value < inMin) value = inMin;
    if (value > inMax) value = inMax;
    // Scale to -32768..0
    return static_cast<long>((inMax - value) * (-32768.0 / (inMax - inMin)) + 32768);
}

std::string Utils::wstringToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return {};

    int sizeNeeded = WideCharToMultiByte(
            CP_UTF8,               // Convert to UTF-8
            0,                     // No special flags
            wstr.c_str(),          // Source wide string
            (int)wstr.size(),      // Number of characters to convert
            nullptr,               // No output yet
            0,                     // Calculate required buffer size
            nullptr, nullptr       // No default char
    );

    std::string strTo(sizeNeeded, 0);
    WideCharToMultiByte(
            CP_UTF8,
            0,
            wstr.c_str(),
            (int)wstr.size(),
            strTo.data(),
            sizeNeeded,
            nullptr, nullptr
    );

    return strTo;
}