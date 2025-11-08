#pragma once
#include "Utils.h"

class IEmulator {
public:
    virtual ~IEmulator() = default;

    // Initialize emulator
    virtual bool initialize() = 0;

    // Update the state of the emulator
    virtual void update(const Utils::MozaState &state, const Utils::Config &config) const = 0;
};
