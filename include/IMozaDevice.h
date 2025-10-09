#pragma once
#include "Utils.h"

class IMozaDevice {
public:
    virtual ~IMozaDevice() = default;

    // Initialize connection to the hardware / SDK
    virtual bool initialize() = 0;

    // Poll new data from the device and update internal state
    virtual void update() = 0;

    // Get the latest device state
    virtual Utils::MozaState getState() = 0;
};