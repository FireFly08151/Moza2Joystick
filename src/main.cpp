#include <thread>
#include <iostream>
#include "Factories.h"

int main() {
    Utils::Config cfg = Utils::loadConfig("config.json");

    auto moza = Utils::createMozaDevice(cfg.backend);
    auto emulator = Utils::createEmulator(cfg.emulator);

    if (!moza->initialize()) {
        std::cerr << "Failed to initialize MOZA device\n";
        return -1;
    }

    if (!emulator->initialize(cfg)) {
        std::cerr << "Failed to initialize emulator\n";
        return 1;
    }

    while (true) {
        moza->update();

        Utils::MozaState state = moza->getState();

        emulator->update(state);

        Utils::printMozaState(state);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}