#pragma once
#include <memory>
#include <string>
#include "IMozaDevice.h"
#include "IEmulator.h"

namespace Utils {
    std::unique_ptr<IMozaDevice> createMozaDevice(const std::string& backend);

    std::unique_ptr<IEmulator> createEmulator(const std::string& emulator);
}
