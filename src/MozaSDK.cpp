#include "MozaSDK.h"
#include <iostream>
#include <cstring>
#include "mozaAPI.h"
#include "Utils.h"

MozaSDK::MozaSDK() {
    std::memset(&currentState, 0, sizeof(Utils::MozaState));
}

MozaSDK::~MozaSDK() {
    moza::removeMozaSDK(); // uninstall SDK when program ends
}


bool MozaSDK::initialize() {
    moza::installMozaSDK(); // initialize the SDK

    // No separate device handle needed in this SDK version
    std::cout << "MOZA SDK installed successfully.\n";
    return true; // always succeed
}


void MozaSDK::update() {
    Utils::MozaState newState{};

    ERRORCODE err = NORMAL;
    const HIDData* d = moza::getHIDData(err);
    if (!d || err != NORMAL) {
        // Failed to read device, keep previous state
        return;
    }

    newState.wheel = static_cast<int16_t>(d->steeringWheelAxle - 32768);
    newState.throttle = d->throttle;
    newState.brake    = d->brake;
    newState.clutch   = d->clutch;

    for (int i = 0; i < 128; ++i) {
        newState.buttons[i] = d->buttons[i].isPressed();
    }

    std::lock_guard<std::mutex> lock(stateMutex);
    currentState = newState;
}


Utils::MozaState MozaSDK::getState() {
    std::lock_guard<std::mutex> lock(stateMutex);
    return currentState;
}
