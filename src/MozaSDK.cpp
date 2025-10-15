#define DIRECTINPUT_VERSION 0x0800
#include "MozaSDK.h"
#include <iostream>
#include <cstring>
#include "mozaAPI.h"
#include "Utils.h"

MozaSDK::MozaSDK() {
    std::memset(&currentState, 0, sizeof(Utils::MozaState));
}

MozaSDK::~MozaSDK() {
    moza::removeMozaSDK(); // terminate SDK when program ends
}

bool MozaSDK::initialize() {
    moza::installMozaSDK(); // initialize the SDK

    std::cout << "MOZA SDK initialized successfully.\n";
    return true;
}


void MozaSDK::update() {
    Utils::MozaState newState{};

    ERRORCODE err = NORMAL;
    const HIDData* d = moza::getHIDData(err);
    if (!d || err != NORMAL) {
        // Failed to read device, keep previous state
        return;
    }

    newState.wheel           = (int16_t)(d->steeringWheelAxle ^ 0x8000);
    newState.clutchCombined  = d->clutchSynthesisShaft;
    newState.clutchLeft      = d->clutchIndependentShaftL;
    newState.clutchRight     = d->clutchIndependentShaftR;
    newState.throttle        = d->throttle;
    newState.clutch          = d->clutch;
    newState.brake           = d->brake;
    newState.handbrake       = d->handbrake;
    newState.buttonHandbrake = d->buttonHandbrake;

    for (int i = 0; i < 128; ++i) {
        newState.buttons[i] = d->buttons[i].isPressed();
    }

    // map buttons 15 & 16 to clutchLeft and clutchRight when fully pressed
    newState.buttons[15] |= newState.clutchLeft >= 32760;
    newState.buttons[16] |= newState.clutchRight >= 32760;

    std::lock_guard<std::mutex> lock(stateMutex);
    currentState = newState;
}

Utils::MozaState MozaSDK::getState() {
    std::lock_guard<std::mutex> lock(stateMutex);
    return currentState;
}
