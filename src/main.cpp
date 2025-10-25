#include "VJoyOutput.h"
#include "ViGEmOutput.h"
#include <thread>
#include <iostream>
#include "MozaFactory.h"

int main() {
    Utils::Config cfg = Utils::loadConfig("config.json");

    auto moza = Utils::createMozaDevice(cfg.backend);

    if (!moza->initialize()) {
        std::cerr << "Failed to initialize MOZA device\n";
        return -1;
    }

    ViGEmOutput vpad;
    if (!vpad.initialize()) {
        std::cerr << "Failed to initialize ViGEm output!\n";
        return 1;
    }

    /*VJoyOutput vjoy(cfg.vJoyDeviceId);
    if (!vjoy.initialize()) {
        std::cerr << "Failed to initialize vJoy device!\n";
        return 1;
    }*/

    while (true) {
        moza->update();

        Utils::MozaState state = moza->getState();

        vpad.update(state,cfg);
        //vjoy.update(state, cfg);

        Utils::printMozaState(state);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}