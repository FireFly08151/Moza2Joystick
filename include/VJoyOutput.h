#pragma once
#include <cstdint>
#include "Utils.h"
#include "IEmulator.h"

class VJoyOutput  : public IEmulator{
public:
    explicit VJoyOutput(uint8_t deviceId = 1);
    ~VJoyOutput() override;

    bool initialize() override;
    void update(const Utils::MozaState &state, const Utils::Config &config) const override;

private:
    uint8_t m_deviceId;
    bool m_initialized;
};
