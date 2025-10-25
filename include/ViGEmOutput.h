#pragma once

#include <windows.h>
#include <ViGEm/Client.h>
#include <ViGEm/Common.h>
#include <iostream>

#include "Utils.h"  // Assuming this declares MozaState and Config

class ViGEmOutput {
public:
    ViGEmOutput() = default;
    ~ViGEmOutput();

    bool initialize();
    void update(const Utils::MozaState &state, const Utils::Config &config) const;

private:
    PVIGEM_CLIENT client = nullptr;
    PVIGEM_TARGET pad = nullptr;
    bool initialized = false;
};
