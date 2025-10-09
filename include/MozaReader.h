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

    bool findDevice();  // Moza's VID
    bool openDevice();
    void closeDevice();
    bool readData(unsigned char *buffer);
    static Utils::MozaState parseReport(const unsigned char *buffer);

    Utils::MozaState currentState{};
    std::mutex stateMutex;
};
