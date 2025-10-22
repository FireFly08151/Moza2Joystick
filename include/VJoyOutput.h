#pragma once
#include <cstdint>
#include "Utils.h"

class VJoyOutput {
public:
    explicit VJoyOutput(uint8_t deviceId = 1);
    ~VJoyOutput();

    bool initialize();
    void update(const Utils::MozaState &state, const Utils::Config &config) const;

private:
    uint8_t m_deviceId;
    bool m_initialized;
};
