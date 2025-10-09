#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Utils {

    using json = nlohmann::json;

    struct MozaState {
        int16_t wheel;
        uint8_t throttle;
        uint8_t brake;
        uint8_t clutch;
        bool buttons[128];  // adjust if you need more buttons
    };

    struct Config {
        std::string backend = "MOZA_SDK"; // default backend
        int vjoyDeviceId = 1;
        std::map<std::string, std::string> axisMappings = {
                {"X", "LeftStickX"},
                {"Y", "LeftStickY"},
                {"Z", "Throttle"}
        };
        std::map<std::string, int> buttonMappings = {
                {"Fire", 1},
                {"Jump", 2},
                {"Menu", 3}
        };
    };

    Config loadConfig(const std::string& filename);

    void saveConfig(const Config& cfg, const std::string& filename);

    void mergeJson(json& target, const json& defaults);

    void printConfig(const Config& $cfg);

    long mapToVJoyAxis(int32_t value, int32_t inMin, int32_t inMax);

    // Optional overload for unsigned ranges like 0..255
    inline long mapToVJoyAxis(uint8_t value) {
        return static_cast<int32_t>(value * 128 - 32768); // 0..255 → -32768..0
    }

    std::string wstringToUtf8(const std::wstring& wstr);
}