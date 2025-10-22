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

    const LONG neutral = 16384;

    SetAxis(neutral, m_deviceId, HID_USAGE_X);
    SetAxis(neutral, m_deviceId, HID_USAGE_Y);
    SetAxis(neutral, m_deviceId, HID_USAGE_Z);
    SetAxis(neutral, m_deviceId, HID_USAGE_RX);
    SetAxis(neutral, m_deviceId, HID_USAGE_RY);
    SetAxis(neutral, m_deviceId, HID_USAGE_RZ);
    SetAxis(neutral, m_deviceId, HID_USAGE_SL0);
    SetAxis(neutral, m_deviceId, HID_USAGE_SL1);

    m_initialized = true;
    return true;
}

void VJoyOutput::update(const Utils::MozaState &state, const Utils::Config &config) const {
    if (!m_initialized) return;

    const int deviceId = config.vJoyDeviceId;

    auto mapAxisValue = [&](const std::string &src, bool inverted) -> LONG {
        int32_t inMin = -32768, inMax = 32767, raw = 0;
        if (src == "Wheel") {
            raw = state.wheel;
        } else if (src == "ClutchCombined") {
            raw = state.clutchCombined;
        } else if (src == "ClutchLeft") {
            raw = state.clutchLeft;
        } else if (src == "ClutchRight") {
            raw = state.clutchRight;
        } else if (src == "Throttle") {
            raw = state.throttle;
        } else if (src == "Clutch") {
            raw = state.clutch;
        } else if (src == "Brake") {
            raw = state.brake;
        } else if (src == "Handbrake") {
            raw = state.handbrake;
        } else {
            return raw; // neutral midpoint for unhandled/unused
        }
        return Utils::mapToVJoyAxis(raw, inMin, inMax, inverted);
    };

    for (const auto &[axisName, mapping] : config.axisMappings) {
        if (mapping.source == "None")
            continue; // Skip unassigned axes entirely

        LONG value = mapAxisValue(mapping.source, mapping.inverted);

        if (axisName == "X")      SetAxis(value, deviceId, HID_USAGE_X);
        else if (axisName == "Y") SetAxis(value, deviceId, HID_USAGE_Y);
        else if (axisName == "Z") SetAxis(value, deviceId, HID_USAGE_Z);
        else if (axisName == "Rx") SetAxis(value, deviceId, HID_USAGE_RX);
        else if (axisName == "Ry") SetAxis(value, deviceId, HID_USAGE_RY);
        else if (axisName == "Rz") SetAxis(value, deviceId, HID_USAGE_RZ);
        else if (axisName == "Sl0") SetAxis(value, deviceId, HID_USAGE_SL0);
        else if (axisName == "Sl1") SetAxis(value, deviceId, HID_USAGE_SL1);
    }

    // Buttons
    for (const auto &[name, index] : config.buttonMappings) {
        int btnIndex = index - 1;
        if (btnIndex >= 0 && btnIndex < 128)
            SetBtn(state.buttons[btnIndex], deviceId, index);
    }
}
