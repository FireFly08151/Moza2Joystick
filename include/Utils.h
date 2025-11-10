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
        int16_t clutchCombined;
        int16_t clutchLeft;
        int16_t clutchRight;
        int16_t throttle;
        int16_t clutch;
        int16_t brake;
        int16_t handbrake;
        // I don't know how to handle this right now, it might already be included in buttons
        bool buttonHandbrake;
        // ToDo implement shifter (activate button on up-/downshift)
        bool buttons[128];
    };

    struct AxisMapping {
        // "Wheel", "ClutchCombined", "ClutchLeft", "ClutchRight",
        // "Throttle", "Clutch", "Brake", "Handbrake", "None"
        std::string source;
        bool inverted = false;
    };

    struct Config {
        std::string backend = "hidapi";
        std::vector<std::string> backend_available = {"hidapi","MOZA_SDK"};
        std::string emulator = "ViGEm";
        std::vector<std::string> emulator_available = {"ViGEm","vJoy"};
        int vJoyDeviceId = 1;
        int stickdeadzone = 0;

        std::map<std::string, Utils::AxisMapping> ViGEmAxisMappings = {
                {"ThumbLX",   {"Wheel",    false}},
                {"ThumbLY",   {"None",   false}},
                {"ThumbRX",   {"None",    false}},
                {"ThumbRY",  {"None", false}},
                {"RightTrigger",  {"Throttle",     false}},
                {"LeftTrigger",  {"Brake",     false}}
        };

        std::map<std::string, int> ViGEmButtonMappings = {
                {"DPad_up",  1},
                {"DPad_down",  2},
                {"DPad_left",  3},
                {"DPad_right",  4},
                {"start",  5},
                {"back",  6},
                {"left_thumb",  7},
                {"right_thumb",  8},
                {"left_shoulder",  9},
                {"right_shoulder", 10},
                {"guide", 11},
                {"A", 12},
                {"B", 13},
                {"X", 14},
                {"Y", 15},
        };

        std::map<std::string, Utils::AxisMapping> vJoyAxisMappings = {
                {"X",   {"Wheel",    false}},
                {"Y",   {"Throttle",   false}},
                {"Z",   {"Brake",    false}},
                {"Rx",  {"ClutchCombined", false}},
                {"Ry",  {"Clutch",     false}},
                {"Rz",  {"Handbrake",     false}},
                {"Sl0", {"ClutchLeft",    false}},
                {"Sl1", {"ClutchRight", false}}
        };

        std::map<std::string, int> vJoyButtonMappings = {
                {"1", 1},
                {"2", 2},
                {"3", 3},
                {"4", 4},
                {"5", 5},
                {"6", 6},
                {"7", 7},
                {"8", 8},
                {"9", 9},
                {"10", 10},
                {"11", 11},
                {"12", 12},
                {"13", 13},
                {"14", 14},
                {"15", 15},
                {"16", 16},
                {"17", 17},
                {"18", 18},
                {"19", 19},
                {"20", 20},
                {"21", 21},
                {"22", 22},
                {"23", 23},
                {"24", 24},
                {"25", 25},
                {"26", 26},
                {"27", 27},
                {"28", 28},
                {"29", 29},
                {"30", 30},
                {"31", 31},
                {"32", 32},
                {"33", 33},
                {"34", 34},
                {"35", 35},
                {"36", 36},
                {"37", 37},
                {"38", 38},
                {"39", 39},
                {"40", 40},
                {"41", 41},
                {"42", 42},
                {"43", 43},
                {"44", 44},
                {"45", 45},
                {"46", 46},
                {"47", 47},
                {"48", 48},
                {"49", 49},
                {"50", 50},
                {"51", 51},
                {"52", 52},
                {"53", 53},
                {"54", 54},
                {"55", 55},
                {"56", 56},
                {"57", 57},
                {"58", 58},
                {"59", 59},
                {"60", 60},
                {"61", 61},
                {"62", 62},
                {"63", 63},
                {"64", 64},
                {"65", 65},
                {"66", 66},
                {"67", 67},
                {"68", 68},
                {"69", 69},
                {"70", 70},
                {"71", 71},
                {"72", 72},
                {"73", 73},
                {"74", 74},
                {"75", 75},
                {"76", 76},
                {"77", 77},
                {"78", 78},
                {"79", 79},
                {"80", 80},
                {"81", 81},
                {"82", 82},
                {"83", 83},
                {"84", 84},
                {"85", 85},
                {"86", 86},
                {"87", 87},
                {"88", 88},
                {"89", 89},
                {"90", 90},
                {"91", 91},
                {"92", 92},
                {"93", 93},
                {"94", 94},
                {"95", 95},
                {"96", 96},
                {"97", 97},
                {"98", 98},
                {"99", 99},
                {"100", 100},
                {"101", 101},
                {"102", 102},
                {"103", 103},
                {"104", 104},
                {"105", 105},
                {"106", 106},
                {"107", 107},
                {"108", 108},
                {"109", 109},
                {"110", 110},
                {"111", 111},
                {"112", 112},
                {"113", 113},
                {"114", 114},
                {"115", 115},
                {"116", 116},
                {"117", 117},
                {"118", 118},
                {"119", 119},
                {"120", 120},
                {"121", 121},
                {"122", 122},
                {"123", 123},
                {"124", 124},
                {"125", 125},
                {"126", 126},
                {"127", 127},
                {"128", 128},

        };
    };

    Config loadConfig(const std::string& filename);

    void saveConfig(const Config& cfg, const std::string& filename);

    void printConfig(const Config& $cfg);

    void printMozaState(MozaState $state);

    int16_t remove_stickdeadzone(int16_t x, int16_t deadzone = 8000);

    std::string wstringToUtf8(const std::wstring& wstr);
}