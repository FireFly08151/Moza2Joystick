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
        bool buttons[128];
    };

    struct AxisMapping {
        std::string source; // e.g. "Wheel", "Throttle", "Brake", "Clutch", "None"
        bool inverted = false;
    };

    struct Config {
        std::string backend = "MOZA_SDK";
        int vjoyDeviceId = 1;

        std::map<std::string, Utils::AxisMapping> axisMappings = {
                {"X",   {"Wheel",    false}},
                {"Y",   {"Clutch",   false}},
                {"Z",   {"Brake",    false}},
                {"Rx",  {"Throttle", false}},
                {"Ry",  {"None",     false}},
                {"Rz",  {"None",     false}},
                {"Sl0", {"Brake",    true}},
                {"Sl1", {"Throttle", true}}
        };

        std::map<std::string, int> buttonMappings = {
                {"A", 1}, {"B", 2}, {"X", 3}, {"Y", 4},
                {"LB", 5}, {"RB", 6}, {"Back", 7}, {"Start", 8},
                {"LS", 9}, {"RS", 10}
        };
    };

    Config loadConfig(const std::string& filename);

    void saveConfig(const Config& cfg, const std::string& filename);

    void printConfig(const Config& $cfg);

    long mapToVJoyAxis(int32_t value, int32_t inMin, int32_t inMax, bool inverted = false);

    // Optional overload for unsigned ranges like 0..255
    inline long mapToVJoyAxis(uint8_t value) {
        return static_cast<int32_t>(value * 128 - 32768); // 0..255 → -32768..0
    }

    std::string wstringToUtf8(const std::wstring& wstr);
}