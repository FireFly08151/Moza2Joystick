#include "VJoyOutput.h"
#include <thread>
#include "MozaReader.h"
#include <iostream>


int main() {
    MozaReader reader;
    if (!reader.findDevice()) {
        std::cerr << "Moza device not found\n";
        return -1;
    }
    if (!reader.openDevice()) {
        std::cerr << "Failed to open Moza device\n";
        return -1;
    }

    VJoyOutput vjoy(1);
    if (!vjoy.initialize()) {
        std::cerr << "Failed to initialize vJoy device!\n";
        return 1;
    }

    unsigned char buffer[64];

    while (true) {
        if (reader.readData(buffer, sizeof(buffer))) {
            MozaState state = MozaReader::parseReport(buffer, sizeof(buffer));
            vjoy.update(state);
            std::cout << "\rWheel: " << state.wheel
                      << " Throttle: " << int(state.throttle)
                      << " Brake: " << int(state.brake)
                      << " Clutch: " << int(state.clutch)
                      << "    " << std::flush;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    reader.closeDevice();
    return 0;
}
