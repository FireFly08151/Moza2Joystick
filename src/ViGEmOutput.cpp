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

bool ViGEmOutput::initialize() {
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
    return true;
}

void ViGEmOutput::update(const Utils::MozaState &state, const Utils::Config &config) const {
    if (!initialized) return;

    const auto deadzone = static_cast<int16_t>(config.stickdeadzone);

    XUSB_REPORT report{};
    XUSB_REPORT_INIT(&report); // sets everything to neutral/zero
    report.wButtons = 0;

    auto mapAxisValue = [&](const std::string& src, bool inverted) -> int16_t {
        static const std::unordered_map<std::string, std::function<int16_t(const Utils::MozaState&)>> axisSources = {
                {"Wheel",          [](const auto& s) { return s.wheel; }},
                {"ClutchCombined", [](const auto& s) { return s.clutchCombined; }},
                {"ClutchLeft",     [](const auto& s) { return s.clutchLeft; }},
                {"ClutchRight",    [](const auto& s) { return s.clutchRight; }},
                {"Throttle",       [](const auto& s) { return s.throttle; }},
                {"Clutch",         [](const auto& s) { return s.clutch; }},
                {"Brake",          [](const auto& s) { return s.brake; }},
                {"Handbrake",      [](const auto& s) { return s.handbrake; }}
        };

        auto it = axisSources.find(src);
        if (it == axisSources.end())
            return 0; // default if unmapped or invalid

        int16_t value = it->second(state);
        if (inverted)
            value = -value;

        return value;
    };


    // Map logical names -> XUSB flags
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

    // Axes
    for (const auto &[axisName, mapping] : config.ViGEmAxisMappings) {
        if (mapping.source == "None") continue;

        int16_t value = mapAxisValue(mapping.source, mapping.inverted);

        /*std::cout << "\r" << static_cast<int>(state.wheel) << ' '
                  << static_cast<int>(Utils::remove_stickdeadzone(state.wheel, deadzone)) << ' '
                  << static_cast<int>((state.throttle * 255) / 65535 + 127) << ' '
                  << static_cast<int>((state.brake * 255) / 65535 + 127) << "        ";*/

        if (axisName == "ThumbLX") report.sThumbLX = static_cast<short>(Utils::remove_stickdeadzone(value, deadzone));
        if (axisName == "ThumbLY") report.sThumbLY = static_cast<short>(Utils::remove_stickdeadzone(value, deadzone));
        if (axisName == "ThumbRX") report.sThumbRX = static_cast<short>(Utils::remove_stickdeadzone(value, deadzone));
        if (axisName == "ThumbRY") report.sThumbRY = static_cast<short>(Utils::remove_stickdeadzone(value, deadzone));
        if (axisName == "RightTrigger") report.bRightTrigger = (value * 255)/65535+127;
        if (axisName == "LeftTrigger") report.bLeftTrigger = (value * 255)/65535+127;
    }

    // Buttons
    for (const auto& [logicalName, xusbFlag] : xusbButtonFlags) {
        auto it = config.ViGEmButtonMappings.find(logicalName);
        if (it == config.ViGEmButtonMappings.end())
            continue; // unmapped

        int physicalIndex = it->second; // e.g. 12 from config
        if (physicalIndex >= 0 && physicalIndex < std::size(state.buttons)) {
            if (state.buttons[physicalIndex])
                report.wButtons |= xusbFlag;
        }
    }

    vigem_target_x360_update(client, pad, report);
}
