#include "MozaFactory.h"
#include "MozaReader.h"
#include "MozaSDK.h"
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

}
