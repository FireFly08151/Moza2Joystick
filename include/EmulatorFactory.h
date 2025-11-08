#pragma once
#include <memory>
#include <string>
#include "IEmulator.h"

namespace Utils {
    std::unique_ptr<IEmulator> createEmulator(const std::string& emulator);
}
