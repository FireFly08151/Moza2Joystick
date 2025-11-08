#pragma once

#include <windows.h>
#include <ViGEm/Client.h>
#include <ViGEm/Common.h>
#include <iostream>

#include "Utils.h"
#include "IEmulator.h"

class ViGEmOutput : public IEmulator{
public:
    ViGEmOutput() = default;
    ~ViGEmOutput() override;

    bool initialize() override;
    void update(const Utils::MozaState &state, const Utils::Config &config) const override;

private:
    PVIGEM_CLIENT client = nullptr;
    PVIGEM_TARGET pad = nullptr;
    bool initialized = false;
};
