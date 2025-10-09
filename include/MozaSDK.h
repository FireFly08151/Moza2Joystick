#pragma once
#include <cstdint>
#include <mutex>
#include "Utils.h"

class MozaSDK {
public:
    MozaSDK();
    ~MozaSDK();

    static bool initialize();
    void update();
    Utils::MozaState getState();

private:
    Utils::MozaState currentState{};
    std::mutex stateMutex;
};
