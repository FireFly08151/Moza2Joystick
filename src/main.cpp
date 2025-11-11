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
    auto lastReportTime = clock::now();
    constexpr int smoothingWindow = 100;
    std::array<double, smoothingWindow> intervalHistory{};
    int intervalIndex = 0;
    double avgInterval = 0;

    while (true) {
        auto readStart = clock::now();
        moza->update();
        auto readEnd = clock::now();

        // ---- Compute report interval ----
        auto now = readEnd;
        double reportIntervalMs = duration<double, std::milli>(now - lastReportTime).count();
        lastReportTime = now;

        // ---- Update moving average ----
        intervalHistory[intervalIndex++] = reportIntervalMs;
        if (intervalIndex >= smoothingWindow) intervalIndex = 0;

        for (double v : intervalHistory) avgInterval += v;
        avgInterval /= smoothingWindow;

        // ---- Process data ----
        auto processStart = clock::now();
        Utils::MozaState state = moza->getState();
        emulator->update(state);
        auto processEnd = clock::now();

        /*double readTimeMs = duration<double, std::milli>(readEnd - readStart).count();
        double processTimeMs = duration<double, std::milli>(processEnd - processStart).count();
        std::cout << "\rHID wait: " << std::fixed << std::setprecision(4) << readTimeMs
                  << " ms, Process Time: " << std::fixed << std::setprecision(4) << processTimeMs
                  << " ms, Interval: " << std::fixed << std::setprecision(4) << reportIntervalMs
                  << " ms, Avg Interval: " << std::fixed << std::setprecision(4) << avgInterval
                  << " ms";*/

        //Utils::printMozaState(state, avgInterval);

        std::cout << "\r"
                  << std::fixed << std::setprecision(4) << avgInterval << "ms"
                  << " Wheel: " << std::setw(6) << std::setfill(' ') << state.wheel
                  << " Throttle: " << std::setw(6)<< std::setfill(' ') << static_cast<int>(state.throttle)
                  << " Brake: " << std::setw(6)<< std::setfill(' ') << static_cast<int>(state.brake)
                  << "    " << std::flush;
    }
}