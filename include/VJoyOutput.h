#pragma once
#include <cstdint>
#include "MozaReader.h" // for MozaState

class VJoyOutput {
public:
    explicit VJoyOutput(uint8_t deviceId = 1);
    ~VJoyOutput();

    bool initialize();
    void update(const MozaState &state);

private:
    uint8_t m_deviceId;
    bool m_initialized;
};
