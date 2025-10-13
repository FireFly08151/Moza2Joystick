#include "VJoyOutput.h"
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

    VJoyOutput vjoy(cfg.vjoyDeviceId);
    if (!vjoy.initialize()) {
        std::cerr << "Failed to initialize vJoy device!\n";
        return 1;
    }

    while (true) {
        moza->update();
        Utils::MozaState state = moza->getState();

        vjoy.update(state, cfg);

        std::cout << "\rWheel: " << std::setw(6) << std::setfill(' ') << state.wheel
                  << " Throttle: " << std::setw(3) << static_cast<int>(state.throttle)
                  << " Brake: " << std::setw(3) << static_cast<int>(state.brake)
                  << " Clutch: " << std::setw(3) << static_cast<int>(state.clutch)
                  << " Buttons: ";

        for (int i = 0; i < 50; ++i) {
            std::cout << (state.buttons[i] ? '1' : '0');
            if (i < 49) std::cout << ' ';
        }

        std::cout << "    " << std::flush;

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}