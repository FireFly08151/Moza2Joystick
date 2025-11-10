#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>
#include "public.h"
#include "vjoyinterface.h"
#include "VJoyOutput.h"
#include "Utils.h"
#include <iostream>


VJoyOutput::VJoyOutput(uint8_t deviceId)
        : m_deviceId(deviceId) {}

VJoyOutput::~VJoyOutput() {
    if (m_initialized)
        RelinquishVJD(m_deviceId);
}

bool VJoyOutput::initialize(const Utils::Config &conf) {
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

    stickDeadzone = static_cast<int>(conf.stickdeadzone);

    preprocessConfig(conf);

    m_initialized = true;
    return true;
}

void VJoyOutput::preprocessConfig(const Utils::Config &conf) {
    axisBindings.clear();
    buttonBindings.clear();

    // --- Axis sources ---
    static const std::unordered_map<std::string, std::function<int16_t(const Utils::MozaState&)>> axisSources = {
            {"Wheel",          [](const auto& s){ return s.wheel; }},
            {"ClutchCombined", [](const auto& s){ return s.clutchCombined; }},
            {"ClutchLeft",     [](const auto& s){ return s.clutchLeft; }},
            {"ClutchRight",    [](const auto& s){ return s.clutchRight; }},
            {"Throttle",       [](const auto& s){ return s.throttle; }},
            {"Clutch",         [](const auto& s){ return s.clutch; }},
            {"Brake",          [](const auto& s){ return s.brake; }},
            {"Handbrake",      [](const auto& s){ return s.handbrake; }}
    };

    // Map logical axis names to vJoy HID usages
    for (const auto& [axisName, mapping] : conf.vJoyAxisMappings) {
        if (mapping.source == "None") continue;

        AxisBinding bind{};
        bind.inverted = mapping.inverted;

        if      (axisName == "X")   bind.target = AxisTarget::X;
        else if (axisName == "Y")   bind.target = AxisTarget::Y;
        else if (axisName == "Z")   bind.target = AxisTarget::Z;
        else if (axisName == "Rx")  bind.target = AxisTarget::Rx;
        else if (axisName == "Ry")  bind.target = AxisTarget::Ry;
        else if (axisName == "Rz")  bind.target = AxisTarget::Rz;
        else if (axisName == "Sl0") bind.target = AxisTarget::Sl0;
        else if (axisName == "Sl1") bind.target = AxisTarget::Sl1;
        else bind.target = AxisTarget::None;

        auto it = axisSources.find(mapping.source);
        if (it != axisSources.end()) bind.getValue = it->second;

        axisBindings.push_back(bind);
    }

    // --- Buttons ---
    for (const auto& [name, index] : conf.vJoyButtonMappings) {
        int btnIndex = index - 1;
        if (btnIndex >= 0 && btnIndex < 128) {
            buttonBindings.push_back({ btnIndex, index });
        }
    }
}

void VJoyOutput::update(const Utils::MozaState &state) const {
    if (!m_initialized) return;

    // --- Update axes ---
    for (const auto& b : axisBindings) {
        if (!b.getValue) continue;

        int16_t value = b.getValue(state);

        if (stickDeadzone != 0 &&
            (b.target == AxisTarget::X || b.target == AxisTarget::Y ||
             b.target == AxisTarget::Z || b.target == AxisTarget::Rx ||
             b.target == AxisTarget::Ry || b.target == AxisTarget::Rz ||
             b.target == AxisTarget::Sl0 || b.target == AxisTarget::Sl1)) {
            value = Utils::remove_stickdeadzone(value, stickDeadzone);
        }

        LONG vJoyValue = mapToVJoyAxis(value, b.inverted);

        switch (b.target) {
            case AxisTarget::X:   SetAxis(vJoyValue, m_deviceId, HID_USAGE_X); break;
            case AxisTarget::Y:   SetAxis(vJoyValue, m_deviceId, HID_USAGE_Y); break;
            case AxisTarget::Z:   SetAxis(vJoyValue, m_deviceId, HID_USAGE_Z); break;
            case AxisTarget::Rx:  SetAxis(vJoyValue, m_deviceId, HID_USAGE_RX); break;
            case AxisTarget::Ry:  SetAxis(vJoyValue, m_deviceId, HID_USAGE_RY); break;
            case AxisTarget::Rz:  SetAxis(vJoyValue, m_deviceId, HID_USAGE_RZ); break;
            case AxisTarget::Sl0: SetAxis(vJoyValue, m_deviceId, HID_USAGE_SL0); break;
            case AxisTarget::Sl1: SetAxis(vJoyValue, m_deviceId, HID_USAGE_SL1); break;
            default: break;
        }
    }

    // --- Update buttons ---
    for (const auto& b : buttonBindings) {
        SetBtn(state.buttons[b.physicalIndex], m_deviceId, b.vJoyButtonIndex);
    }
}
