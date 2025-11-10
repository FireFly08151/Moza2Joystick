#pragma once

#include <windows.h>
#include <ViGEm/Client.h>
#include <ViGEm/Common.h>
#include <iostream>
#include <vector>
#include <functional>
#include <unordered_map>

#include "Utils.h"
#include "IEmulator.h"

class ViGEmOutput : public IEmulator {
public:
    ViGEmOutput() = default;
    ~ViGEmOutput() override;

    // Initialize with config, precomputes axes and buttons
    bool initialize(const Utils::Config &config) override;

    // Update the controller state
    void update(const Utils::MozaState &state) const override;

private:
    PVIGEM_CLIENT client = nullptr;
    PVIGEM_TARGET pad = nullptr;
    bool initialized = false;

    // --- Axis mapping ---
    enum class AxisTarget {
        None,
        ThumbLX,
        ThumbLY,
        ThumbRX,
        ThumbRY,
        LeftTrigger,
        RightTrigger
    };

    struct AxisBinding {
        AxisTarget target = AxisTarget::None;
        std::function<int16_t(const Utils::MozaState&)> getValue;
        bool inverted = false;
    };
    std::vector<AxisBinding> axisBindings;

    // --- Button mapping ---
    struct ButtonBinding {
        WORD xusbFlag;
        int physicalIndex;
    };
    std::vector<ButtonBinding> buttonBindings;

    int16_t stickDeadzone = 0;

    // --- Helpers ---
    void preprocessConfig(const Utils::Config &config);

    static inline uint8_t fast_map(int16_t value) {
        uint32_t v = static_cast<int32_t>(value) + 32768;
        return static_cast<uint8_t>((v * 255u + 32767u) >> 16);
    }
};
