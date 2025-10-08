#include "MozaReader.h"
#include "Utils.h"
#include <iostream>
#include <sstream>
#include <string>

MozaReader::MozaReader() {
    hid_init();
}

MozaReader::~MozaReader() {
    closeDevice();
    hid_exit();
}

bool MozaReader::findDevice(unsigned short vendor_id) {
    struct hid_device_info* devs = hid_enumerate(vendor_id, 0x0000);
    struct hid_device_info* cur = devs;

    bool found = false;
    while (cur) {
        std::wcout << L"Found device: " << cur->manufacturer_string
                   << L" | " << cur->product_string
                   << L" | VID: " << std::hex << cur->vendor_id
                   << L" | PID: " << cur->product_id
                   << std::endl;

        // Look for likely Moza base or wheel (R9, R16, etc.)
        if (cur->vendor_id == vendor_id) {
            deviceInfo = {
                    cur->manufacturer_string ? cur->manufacturer_string : L"",
                    cur->product_string ? cur->product_string : L"",
                    cur->vendor_id,
                    cur->product_id,
                    cur->path ? std::wstring(cur->path, cur->path + strlen(cur->path)) : L""
            };
            found = true;
            break;
        }
        cur = cur->next;
    }

    hid_free_enumeration(devs);
    return found;
}

bool MozaReader::openDevice() {
    if (deviceInfo.path.empty()) {
        std::wcerr << L"No device path available to open.\n";
        return false;
    }

    std::string pathUtf8 = Utils::wstringToUtf8(deviceInfo.path);
    deviceHandle = hid_open_path(pathUtf8.c_str());

    if (!deviceHandle) {
        std::wcerr << L"Failed to open Moza device.\n";
        return false;
    }

    std::wcout << L"Opened Moza device successfully.\n";
    return true;
}

void MozaReader::closeDevice() {
    if (deviceHandle) {
        hid_close(deviceHandle);
        deviceHandle = nullptr;
    }
}

bool MozaReader::readData(unsigned char* buffer, size_t length) {
    if (!deviceHandle) return false;

    int res = hid_read(deviceHandle, buffer, static_cast<size_t>(length));
    if (res < 0) {
        return false;
    }
    return true;
}

MozaState MozaReader::parseReport(const unsigned char *buffer, size_t length) {
    MozaState state{};
    if (length < 10) return state;

    // Example mapping (update after testing)
    uint16_t rawWheel = buffer[1] | (buffer[2] << 8); // 0..65535
    auto wheel = static_cast<int16_t>(rawWheel - 32768); // -32768..32767
    state.wheel = wheel;
    //state.wheel = static_cast<int16_t>((buffer[1] << 8) | buffer[2]);
    //state.wheel = buffer[1] | (buffer[2] << 8);
    state.throttle = buffer[3];
    state.brake = buffer[4];
    state.clutch = buffer[5];

    uint16_t btnBits = buffer[6] | (buffer[7] << 8);
    for (int i = 0; i < 16; ++i)
        state.buttons[i] = (btnBits >> i) & 0x1;

    return state;
}
