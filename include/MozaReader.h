#pragma once
#include <string>
#include <vector>
#include <hidapi.h>
#include "Utils.h"
#include <mutex>
#include "IMozaDevice.h"

struct MozaDeviceInfo {
    std::wstring manufacturer;
    std::wstring product;
    std::wstring path;
};

struct DPad {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
};

class MozaReader : public IMozaDevice {
public:
    MozaReader();
    ~MozaReader() override;

    bool initialize() override;
    void update() override;
    Utils::MozaState getState() override;

private:
    hid_device* deviceHandle = nullptr;
    MozaDeviceInfo deviceInfo;

    bool findDevice();
    bool openDevice();
    void closeDevice();
    bool readData(unsigned char *buffer);
    static Utils::MozaState parseReport(const unsigned char *buffer);
    static DPad parse_dpad(unsigned char b);

    Utils::MozaState currentState{};
    std::mutex stateMutex;
};
