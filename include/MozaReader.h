#pragma once
#include <string>
#include <vector>
#include <hidapi.h>

struct MozaDeviceInfo {
    std::wstring manufacturer;
    std::wstring product;
    unsigned short vendor_id;
    unsigned short product_id;
    std::wstring path;
};

struct MozaState {
    int16_t wheel;
    uint8_t throttle;
    uint8_t brake;
    uint8_t clutch;
    bool buttons[16];  // adjust if you need more buttons
};

class MozaReader {
public:
    MozaReader();
    ~MozaReader();

    bool findDevice(unsigned short vendor_id = 0x346E);  // Moza's VID
    bool openDevice();
    void closeDevice();
    bool readData(unsigned char* buffer, size_t length);
    static MozaState parseReport(const unsigned char* buffer, size_t length);

private:
    hid_device* deviceHandle = nullptr;
    MozaDeviceInfo deviceInfo;
};
