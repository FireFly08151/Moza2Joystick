#pragma once
#include <memory>
#include <string>
#include "IMozaDevice.h"

namespace Utils {
    std::unique_ptr<IMozaDevice> createMozaDevice(const std::string& backend);
}
