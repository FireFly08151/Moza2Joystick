#pragma once
#include "Utils.h"

class IEmulator {
public:
    virtual ~IEmulator() = default;

    // Initialize emulator
    virtual bool initialize(const Utils::Config &config) = 0;

    // Update the state of the emulator
    virtual void update(const Utils::MozaState &state) const = 0;
};
