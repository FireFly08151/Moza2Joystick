#pragma once
#include <cstdint>
#include <mutex>
#include "Utils.h"
#include "IMozaDevice.h"

class MozaSDK : public IMozaDevice{
public:
    MozaSDK();
    ~MozaSDK() override;

    bool initialize() override;
    void update() override;
    Utils::MozaState getState() override;

private:
    Utils::MozaState currentState{};
    std::mutex stateMutex;
};
