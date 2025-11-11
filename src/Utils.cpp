#include "Utils.h"
#include "windows.h"

namespace Utils {

    using json = nlohmann::json;


    // Tell nlohmann::json how to serialize AxisMapping
    inline void to_json(nlohmann::json &j, const AxisMapping &a) {
        j = nlohmann::json{{"source", a.source}, {"inverted", a.inverted}};
    }

    // Tell nlohmann::json how to deserialize AxisMapping
    inline void from_json(const nlohmann::json &j, AxisMapping &a) {
        j.at("source").get_to(a.source);
        if (j.contains("inverted"))
            j.at("inverted").get_to(a.inverted);
        else
            a.inverted = false;
    }

    // Needed to print AxisMapping
    inline std::ostream& operator<<(std::ostream &os, const AxisMapping &m) {
        os << m.source << (m.inverted ? " (inverted)" : "");
        return os;
    }

    Config loadConfig(const std::string& filename) {

        Config cfg;

        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "[Config] File missing. Using defaults and creating file.\n";
                saveConfig(cfg, filename);
                return cfg;
            }

            nlohmann::json j;
            file >> j;
            //std::cout << "[DEBUG] JSON content on load: " << j.dump(4) << "\n";

            // Backend
            if (j.contains("backend") && j["backend"].contains("selected"))
                cfg.backend = j["backend"]["selected"].get<std::string>();

            // Emulator
            if (j.contains("emulator") && j["emulator"].contains("selected"))
                cfg.emulator = j["emulator"]["selected"].get<std::string>();

            if (j.contains("stickdeadzone"))
                cfg.stickdeadzone = j["stickdeadzone"].get<int>();

            // ViGEm
            if (j.contains("ViGEm")) {
                const auto& v = j["ViGEm"];

                // Axis mappings
                if (v.contains("axis_mappings")) {
                    for (auto& [axis, mapping] : v["axis_mappings"].items()) {
                        cfg.ViGEmAxisMappings[axis] = mapping.get<Utils::AxisMapping>();
                    }
                }

                // Button mappings (handles "None" and missing)
                if (v.contains("button_mappings")) {
                    for (auto& [action, button] : v["button_mappings"].items()) {
                        if (button.is_number_integer()) {
                            cfg.ViGEmButtonMappings[action] = button.get<int>();
                        } else {
                            cfg.ViGEmButtonMappings[action] = -1; // fallback
                        }
                    }
                }
            }

            // vJoy
            if (j.contains("vJoy")) {
                if (j["vJoy"].contains("device_id"))
                    cfg.vJoyDeviceId = j["vJoy"]["device_id"].get<int>();

                if (j["vJoy"].contains("axis_mappings")) {
                    for (auto& [axis, mapping] : j["vJoy"]["axis_mappings"].items()) {
                        cfg.vJoyAxisMappings[axis] = mapping.get<Utils::AxisMapping>();
                    }
                }

                if (j["vJoy"].contains("button_mappings"))
                    for (auto& [action, button] : j["vJoy"]["button_mappings"].items())
                        cfg.vJoyButtonMappings[action] = button.get<int>();
            }

        } catch (const std::exception& e) {
            std::cerr << "[Config] Error reading or parsing config: " << e.what()
                      << ". Using defaults.\n";
            saveConfig(cfg, filename);
        }

        printConfig(cfg);

        return cfg;
    }


    void saveConfig(const Config &cfg, const std::string &filename) {
        json j;
        j["backend"]["selected"] = cfg.backend;
        j["backend"]["available"] = cfg.backend_available;
        j["emulator"]["selected"] = cfg.emulator;
        j["emulator"]["available"] = cfg.emulator_available;
        j["stickdeadzone"] = cfg.stickdeadzone;
        j["ViGEm"]["axis_mappings"] = cfg.ViGEmAxisMappings;
        j["ViGEm"]["button_mappings"] = cfg.ViGEmButtonMappings;
        j["vJoy"]["device_id"] = cfg.vJoyDeviceId;
        j["vJoy"]["axis_mappings"] = cfg.vJoyAxisMappings;
        j["vJoy"]["button_mappings"] = cfg.vJoyButtonMappings;

        std::ofstream file(filename);
        file << std::setw(4) << j << std::endl;
    }

    void printConfig(const Config& $cfg) {
        std::cout << "Selected backend: " << $cfg.backend << "\n";
        std::cout << "Selected emulator: " << $cfg.emulator << "\n";
        if ($cfg.emulator == "vJoy") {
            std::cout << "vJoy device ID: " << $cfg.vJoyDeviceId << "\n";

            std::cout << "vJoy Axis mappings:\n";
            for (auto& [axis, mapping] : $cfg.vJoyAxisMappings)
                std::cout << "  " << axis << " -> " << mapping << "\n";

            /*std::cout << "vJoy Button mappings:\n";
            for (auto& [action, button] : $cfg.vJoyButtonMappings)
                std::cout << "  " << action << " -> " << button << "\n";*/
        }

        if ($cfg.emulator == "ViGEm") {
            std::cout << "ViGEm Axis mappings:\n";
            for (auto &[axis, mapping]: $cfg.ViGEmAxisMappings)
                std::cout << "  " << axis << " -> " << mapping << "\n";

            std::cout << "ViGEm Button mappings:\n";
            for (auto &[action, button]: $cfg.ViGEmButtonMappings)
                std::cout << "  " << action << " -> " << button << "\n";
        }

        std::cout << "Stickdeadzone: " << $cfg.stickdeadzone << "\n";
    }

    void printMozaState(const MozaState $state) {
        std::cout << "\rWheel: " << std::setw(6) << std::setfill(' ') << $state.wheel
                  << " Throttle: " << std::setw(6)<< std::setfill(' ') << static_cast<int>($state.throttle)
                  << " Brake: " << std::setw(6)<< std::setfill(' ') << static_cast<int>($state.brake)
                  << " Clutch: " << std::setw(6) << std::setfill(' ') << static_cast<int>($state.clutchCombined)
                  << " Buttons: ";

        for (int i = 0; i < 50; ++i) {
            std::cout << ($state.buttons[i] ? '1' : '0');
            if (i < 49) std::cout << ' ';
        }

        std::cout << "    " << std::flush;
    }

    void printMozaState(const MozaState $state, double ms) {
        std::cout << "\r"
                  << std::fixed << std::setprecision(4) << ms << "ms"
                  << " Wheel: " << std::setw(6) << std::setfill(' ') << $state.wheel
                  << " Throttle: " << std::setw(6)<< std::setfill(' ') << static_cast<int>($state.throttle)
                  << " Brake: " << std::setw(6)<< std::setfill(' ') << static_cast<int>($state.brake)
                  << " Clutch: " << std::setw(6) << std::setfill(' ') << static_cast<int>($state.clutchCombined)
                  << " Buttons: ";

        for (int i = 0; i < 50; ++i) {
            std::cout << ($state.buttons[i] ? '1' : '0');
            if (i < 49) std::cout << ' ';
        }

        std::cout << "    " << std::flush;
    }

    uint8_t mapToByte(int16_t value, int16_t inMin, int16_t inMax) {
        if (inMin == inMax) return 0; // avoid division by zero

        // Clamp value to the input range
        value = std::clamp(value, inMin, inMax);

        // Map value from [inMin, inMax] -> [0, 255]
        int result = (value - inMin) * 255 / (inMax - inMin);

        return static_cast<uint8_t>(result);
    }

    /**
     * Counteracts the deadzone implemented in some games by scaling the values to be outside the deadzone.
     * It's not 100% accurate, but faster than doing it properly ;D
     * @param x Input value which gets adjusted
     * @param deadzone defaults to 8000 (GTA V stick deadzone)
     * @return - the adjusted value
     */
    int16_t remove_stickDeadzone(int16_t x, int16_t deadzone) {
        if (deadzone == 0) return x; // exact preservation

        constexpr int32_t MAX_VAL = 32767;

        // branchless abs() for int16_t
        int32_t sign = (x > 0) - (x < 0);
        int32_t absx = (x ^ (x >> 15)) - (x >> 15);

        int32_t scaled = deadzone + ((absx * (MAX_VAL - deadzone)) >> 15); // >>15 ≈ /32768

        // clamp just in case
        if (scaled > MAX_VAL) scaled = MAX_VAL;

        return static_cast<int16_t>(sign * scaled);
    }

    std::string wstringToUtf8(const std::wstring &wstr) {
        if (wstr.empty()) return {};

        int sizeNeeded = WideCharToMultiByte(
                CP_UTF8,               // Convert to UTF-8
                0,                     // No special flags
                wstr.c_str(),          // Source wide string
                (int) wstr.size(),      // Number of characters to convert
                nullptr,               // No output yet
                0,                     // Calculate required buffer size
                nullptr, nullptr       // No default char
        );

        std::string strTo(sizeNeeded, 0);
        WideCharToMultiByte(
                CP_UTF8,
                0,
                wstr.c_str(),
                (int) wstr.size(),
                strTo.data(),
                sizeNeeded,
                nullptr, nullptr
        );

        return strTo;
    }
}