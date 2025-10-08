#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>
#include "public.h"
#include "vjoyinterface.h"
#include "VJoyOutput.h"
#include "Utils.h"
#include <iostream>


VJoyOutput::VJoyOutput(uint8_t deviceId)
        : m_deviceId(deviceId), m_initialized(false) {}

VJoyOutput::~VJoyOutput() {
    if (m_initialized)
        RelinquishVJD(m_deviceId);
}

bool VJoyOutput::initialize() {
    if (!vJoyEnabled()) {
        std::cerr << "vJoy driver not enabled!\n";
        return false;
    }

    if (!AcquireVJD(m_deviceId)) {
        std::cerr << "Failed to acquire vJoy device " << int(m_deviceId) << "\n";
        return false;
    }

    m_initialized = true;
    return true;
}

void VJoyOutput::update(const MozaState &state) {
    if (!m_initialized) return;

    // Wheel: map -32768..32767 → 0..65535
    //long wheelVal = static_cast<long>(state.wheel / 2 + 16384);
    //SetAxis(wheelVal, m_deviceId, HID_USAGE_X);
    SetAxis(Utils::mapToVJoyAxis(state.wheel, -32768, 32768), m_deviceId, HID_USAGE_X);

    // Pedals: scale 0..255 → 0..32767
    SetAxis(state.throttle * 128, m_deviceId, HID_USAGE_RZ);
    SetAxis(state.brake * 128, m_deviceId, HID_USAGE_Y);
    SetAxis(state.clutch * 128, m_deviceId, HID_USAGE_Z);

    // Buttons
    for (int i = 0; i < 16; ++i)
        SetBtn(state.buttons[i] != 0, m_deviceId, i + 1);
}
