#include "Utils.h"

long Utils::mapToVJoyAxis(int32_t value, int32_t inMin, int32_t inMax) {
    if (value < inMin) value = inMin;
    if (value > inMax) value = inMax;
    // Scale to -32768..0
    return static_cast<long>((inMax - value) * (-32768.0 / (inMax - inMin)) + 32768);
}
