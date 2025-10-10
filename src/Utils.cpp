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

            // vJoy
            if (j.contains("vJoy")) {
                if (j["vJoy"].contains("device_id"))
                    cfg.vjoyDeviceId = j["vJoy"]["device_id"].get<int>();

                if (j["vJoy"].contains("axis_mappings")) {
                    for (auto& [axis, mapping] : j["vJoy"]["axis_mappings"].items()) {
                        cfg.axisMappings[axis] = mapping.get<Utils::AxisMapping>();
                    }
                }

                if (j["vJoy"].contains("button_mappings"))
                    for (auto& [action, button] : j["vJoy"]["button_mappings"].items())
                        cfg.buttonMappings[action] = button.get<int>();
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
        j["vJoy"]["device_id"] = cfg.vjoyDeviceId;
        j["vJoy"]["axis_mappings"] = cfg.axisMappings;
        j["vJoy"]["button_mappings"] = cfg.buttonMappings;

        std::ofstream file(filename);
        file << std::setw(4) << j << std::endl;
    }

    void printConfig(const Config& $cfg) {
        std::cout << "Selected backend: " << $cfg.backend << "\n";
        std::cout << "vJoy device ID: " << $cfg.vjoyDeviceId << "\n";

        std::cout << "Axis mappings:\n";
        for (auto& [axis, mapping] : $cfg.axisMappings)
            std::cout << "  " << axis << " -> " << mapping << "\n";

        std::cout << "Button mappings:\n";
        for (auto& [action, button] : $cfg.buttonMappings)
            std::cout << "  " << action << " -> " << button << "\n";
    }

    long mapToVJoyAxis(int32_t value, int32_t inMin, int32_t inMax, bool inverted) {
        if (value < inMin) value = inMin;
        if (value > inMax) value = inMax;
        // Scale to 0..32768
        double scaled = ((inMax - value) * (-32768.0 / (inMax - inMin)) + 32768);
        return static_cast<long>(inverted ? 32768.0-scaled : scaled);
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