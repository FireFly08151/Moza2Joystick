#include <windows.h>
#include "ViGEmOutput.h"

ViGEmOutput::~ViGEmOutput() {
    if (pad) {
        vigem_target_remove(client, pad);
        vigem_target_free(pad);
    }
    if (client) {
        vigem_disconnect(client);
        vigem_free(client);
    }
}

bool ViGEmOutput::initialize(const Utils::Config &config) {
    client = vigem_alloc();
    if (!client) {
        std::cerr << "ViGEmOutput: Failed to allocate ViGEm client\n";
        return false;
    }

    const auto connectRes = vigem_connect(client);
    if (!VIGEM_SUCCESS(connectRes)) {
        std::cerr << "ViGEmOutput: Failed to connect to ViGEmBus (error 0x"
                  << std::hex << connectRes << ")\n";
        vigem_free(client);
        client = nullptr;
        return false;
    }

    pad = vigem_target_x360_alloc();
    const auto addRes = vigem_target_add(client, pad);
    if (!VIGEM_SUCCESS(addRes)) {
        std::cerr << "ViGEmOutput: Failed to add virtual Xbox 360 controller (error 0x"
                  << std::hex << addRes << ")\n";
        vigem_target_free(pad);
        vigem_disconnect(client);
        vigem_free(client);
        client = nullptr;
        return false;
    }

    initialized = true;
    std::cout << "ViGEmOutput: Virtual Xbox 360 controller initialized successfully\n";

    // Store deadzone
    stickDeadzone = static_cast<int16_t>(config.stickdeadzone);

    // Precompute axes and buttons
    preprocessConfig(config);

    return true;
}

void ViGEmOutput::preprocessConfig(const Utils::Config &config) {
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

    // Map logical axis names to target fields
    for (const auto& [axisName, mapping] : config.ViGEmAxisMappings) {
        if (mapping.source == "None") continue;

        AxisBinding bind{};
        bind.inverted = mapping.inverted;

        // map target by axisName
        if      (axisName == "ThumbLX")      bind.target = AxisTarget::ThumbLX;
        else if (axisName == "ThumbLY")      bind.target = AxisTarget::ThumbLY;
        else if (axisName == "ThumbRX")      bind.target = AxisTarget::ThumbRX;
        else if (axisName == "ThumbRY")      bind.target = AxisTarget::ThumbRY;
        else if (axisName == "LeftTrigger")  bind.target = AxisTarget::LeftTrigger;
        else if (axisName == "RightTrigger") bind.target = AxisTarget::RightTrigger;
        else bind.target = AxisTarget::None;

        auto it = axisSources.find(mapping.source);
        if (it != axisSources.end()) bind.getValue = it->second;
        else bind.getValue = nullptr;

        axisBindings.push_back(bind);
    }

    // --- Button flags ---
    static const std::unordered_map<std::string, WORD> xusbButtonFlags = {
            {"DPad_up", XUSB_GAMEPAD_DPAD_UP},
            {"DPad_down", XUSB_GAMEPAD_DPAD_DOWN},
            {"DPad_left", XUSB_GAMEPAD_DPAD_LEFT},
            {"DPad_right", XUSB_GAMEPAD_DPAD_RIGHT},
            {"start", XUSB_GAMEPAD_START},
            {"back", XUSB_GAMEPAD_BACK},
            {"left_thumb", XUSB_GAMEPAD_LEFT_THUMB},
            {"right_thumb", XUSB_GAMEPAD_RIGHT_THUMB},
            {"left_shoulder", XUSB_GAMEPAD_LEFT_SHOULDER},
            {"right_shoulder", XUSB_GAMEPAD_RIGHT_SHOULDER},
            {"guide", XUSB_GAMEPAD_GUIDE},
            {"A", XUSB_GAMEPAD_A},
            {"B", XUSB_GAMEPAD_B},
            {"X", XUSB_GAMEPAD_X},
            {"Y", XUSB_GAMEPAD_Y}
    };

    // Precompute button bindings
    for (const auto& [logicalName, physicalIndex] : config.ViGEmButtonMappings) {
        auto it = xusbButtonFlags.find(logicalName);
        if (it != xusbButtonFlags.end() && physicalIndex >= 0) {
            buttonBindings.push_back({ it->second, physicalIndex });
        }
    }
}

void ViGEmOutput::update(const Utils::MozaState &state) const {
    if (!initialized) return;

    XUSB_REPORT report{};
    XUSB_REPORT_INIT(&report);
    report.wButtons = 0;

    // --- Update axes ---
    for (const auto &b : axisBindings) {
        if (!b.getValue) continue;

        int16_t value = b.getValue(state);
        if (b.inverted) value = 65535 - value;

        // apply deadzone to sticks only
        if (stickDeadzone != 0 &&
            (b.target == AxisTarget::ThumbLX || b.target == AxisTarget::ThumbLY ||
             b.target == AxisTarget::ThumbRX || b.target == AxisTarget::ThumbRY)) {
            value = Utils::remove_stickDeadzone(value, stickDeadzone);
        }

        switch (b.target) {
            case AxisTarget::ThumbLX:     report.sThumbLX = value; break;
            case AxisTarget::ThumbLY:     report.sThumbLY = value; break;
            case AxisTarget::ThumbRX:     report.sThumbRX = value; break;
            case AxisTarget::ThumbRY:     report.sThumbRY = value; break;
            case AxisTarget::LeftTrigger: report.bLeftTrigger  = fast_map(value); break;
            case AxisTarget::RightTrigger:report.bRightTrigger = fast_map(value); break;
            default: break;
        }
    }

    // --- Update buttons ---
    for (const auto &b : buttonBindings) {
        if (b.physicalIndex < std::size(state.buttons) && state.buttons[b.physicalIndex])
            report.wButtons |= b.xusbFlag;
    }

    /*std::cout << "\r" << static_cast<int>(state.wheel) << ' '
              << static_cast<int>(Utils::remove_stickDeadzone(state.wheel, stickDeadzone)) << ' '
              << static_cast<int>(report.sThumbLX) << ' '
              << static_cast<int>(fast_map(state.throttle)) << ' '
              << static_cast<int>(fast_map(state.brake)) << "        ";*/

    vigem_target_x360_update(client, pad, report);
}
