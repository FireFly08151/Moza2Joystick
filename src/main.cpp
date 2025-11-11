#include <thread>
#include <iostream>
#include "Factories.h"

int main() {
    Utils::Config cfg = Utils::loadConfig("config.json");

    auto moza = Utils::createMozaDevice(cfg.backend);
    auto emulator = Utils::createEmulator(cfg.emulator);

    if (!moza->initialize()) {
        std::cerr << "Failed to initialize MOZA device\n";
        return -2;
    }

    if (!emulator->initialize(cfg)) {
        std::cerr << "Failed to initialize emulator\n";
        return -1;
    }

    using clock = std::chrono::high_resolution_clock;
    using namespace std::chrono;

    const int print_interval = 20;
    double total_ms = 0.0;
    int count = 0;
    double avg_ms = 0;

    while (true) {
        auto start = clock::now();

        moza->update();

        Utils::MozaState state = moza->getState();

        emulator->update(state);

        auto end = clock::now();
        duration<double, std::milli> elapsed = end - start;

        total_ms += elapsed.count();
        count++;

        if (count % print_interval == 0) {
            avg_ms = total_ms / print_interval;
            total_ms = 0.0;
        }

        Utils::printMozaState(state, avg_ms);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}