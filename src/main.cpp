#include "VJoyOutput.h"
#include <thread>
#include "MozaSDK.h"
#include <iostream>
#include "IMozaDevice.h"

int main() {
    // Choose implementation
    std::unique_ptr<IMozaDevice> moza = std::make_unique<MozaSDK>();
    // or: std::unique_ptr<IMozaDevice> moza = std::make_unique<MozaReader>();

    if (!moza->initialize()) {
        std::cerr << "Failed to initialize MOZA device\n";
        return -1;
    }

    VJoyOutput vjoy(1);
    if (!vjoy.initialize()) {
        std::cerr << "Failed to initialize vJoy device!\n";
        return 1;
    }

    while (true) {
        moza->update();
        Utils::MozaState state = moza->getState();

        vjoy.update(state);

        std::cout << "\rWheel: " << state.wheel
                  << " Throttle: " << state.throttle
                  << " Brake: " << state.brake
                  << " Clutch: " << state.clutch
                  << "    " << std::flush;

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
