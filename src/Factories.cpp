#include "Factories.h"
#include "MozaReader.h"
#include "MozaSDK.h"
#include "ViGEmOutput.h"
#include "VJoyOutput.h"
#include <iostream>


namespace Utils {

    std::unique_ptr<IMozaDevice> createMozaDevice(const std::string& backend) {
        if (backend == "MOZA_SDK") {
            std::cout << "Using MozaSDK backend\n";
            return std::make_unique<MozaSDK>();
        }
        else if (backend == "hidapi") {
            std::cout << "Using MozaReader (hidapi) backend\n";
            return std::make_unique<MozaReader>();
        }
        else {
            std::cerr << "Unknown backend: " << backend << " — defaulting to MOZA_SDK\n";
            return std::make_unique<MozaSDK>();
        }
    }

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
