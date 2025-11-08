#include "EmulatorFactory.h"
#include "ViGEmOutput.h"
#include "VJoyOutput.h"
#include <iostream>

namespace Utils {

    std::unique_ptr<IEmulator> createEmulator(const std::string& emulator) {
        if (emulator == "ViGEm") {
            std::cout << "Using ViGEm emulator\n";
            return std::make_unique<ViGEmOutput>();
        }
        else if (emulator == "vJoy") {
            std::cout << "Using vJoy emulator\n";
            return std::make_unique<VJoyOutput>();
        }
        else {
            std::cerr << "Unknown emulator: " << emulator << " — defaulting to ViGEm\n";
            return std::make_unique<ViGEmOutput>();
        }
    }

}
