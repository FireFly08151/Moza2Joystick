#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include "Utils.h"
#include "IEmulator.h"

class VJoyOutput : public IEmulator {
public:
    explicit VJoyOutput(uint8_t deviceId = 1);
    ~VJoyOutput() override;

    bool initialize(const Utils::Config &conf) override;
    void update(const Utils::MozaState &state) const override;

private:
    uint8_t m_deviceId;
    bool m_initialized = false;

    int stickDeadzone = 0;

    // --- Axis mapping ---
    enum class AxisTarget {
        None,
        X, Y, Z,
        Rx, Ry, Rz,
        Sl0, Sl1
    };

    struct AxisBinding {
        AxisTarget target = AxisTarget::None;
        std::function<int16_t(const Utils::MozaState&)> getValue;
        bool inverted = false;
    };
    std::vector<AxisBinding> axisBindings;

    // --- Button mapping ---
    struct ButtonBinding {
        int physicalIndex; // index into state.buttons
        int vJoyButtonIndex; // 1-based vJoy button index
    };
    std::vector<ButtonBinding> buttonBindings;

    void preprocessConfig(const Utils::Config &conf);

    static inline LONG mapToVJoyAxis(int16_t value, bool inverted = false) {
        int32_t v = static_cast<int32_t>(value) + 32768; // now 0..65535
        if (inverted) v = 65535 - v;
        // Scale to vJoy range 0..32767
        return static_cast<LONG>(v / 2); // 65536 / 2 = 32768
    }

};
