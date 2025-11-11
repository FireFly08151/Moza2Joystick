#include "MozaReader.h"
#include "Utils.h"
#include <iostream>
#include <sstream>
#include <string>
#ifdef _WIN32
#include <locale>
#include <codecvt>
#endif

MozaReader::MozaReader() {
    hid_init();
}

MozaReader::~MozaReader() {
    closeDevice();
    hid_exit();
}

bool MozaReader::initialize() {
    if (!findDevice()) {
        std::cerr << "Moza device not found.\n";
        return false;
    }
    if (!openDevice()) {
        std::cerr << "Failed to open Moza device.\n";
        return false;
    }
    hid_set_nonblocking(deviceHandle, 0);

    std::cout << "Moza device initialized successfully.\n";
    return true;
}

void MozaReader::update() {
    if (!deviceHandle)
        return;

    unsigned char buffer[48];


    if (readData(buffer)) {
        Utils::MozaState newState = parseReport(buffer);

        std::lock_guard<std::mutex> lock(stateMutex);
        currentState = newState;
    } else {
        static int errorCount = 0;
        if (++errorCount < 10)
            std::cerr << "Failed to read from Moza device.\n";
    }
}

Utils::MozaState MozaReader::getState() {
    std::lock_guard<std::mutex> lock(stateMutex);
    return currentState;
}

bool MozaReader::findDevice() {
    struct hid_device_info* devs = hid_enumerate(0x346E, 0x0000); // Moza vendor ID
    struct hid_device_info* cur = devs;

    bool found = false;
    while (cur) {
        std::wcout << L"Found device:\n"
                   << L"  Manufacturer : " << (cur->manufacturer_string ? cur->manufacturer_string : L"(unknown)") << L"\n"
                   << L"  Product      : " << (cur->product_string ? cur->product_string : L"(unknown)") << L"\n"
                   << L"  VID:PID      : " << std::hex << std::setw(4) << std::setfill(L'0')
                   << cur->vendor_id << L":" << cur->product_id << std::dec << L"\n"
                   << L"  Interface #  : " << cur->interface_number << L"\n"
                   << L"  Path         : " << (cur->path ? cur->path : "(no path)") << L"\n"
                   << std::endl;

        // Look for likely Moza base (R9, R12, etc.)
        if (cur->vendor_id == 0x346E) { // 13422 decimal
            deviceInfo = {
                    cur->manufacturer_string ? cur->manufacturer_string : L"",
                    cur->product_string ? cur->product_string : L"",
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

// prints byte in binary (8 bits)
static std::string byte_to_bits(unsigned char b) {
    std::string s;
    for (int i = 0; i < 8; ++i) {
        s.push_back((b & (1 << i)) ? '1' : '0');
    }
    return s;
}

bool MozaReader::readData(unsigned char *buffer) {
    const size_t bufferSize = 48;
    std::fill(buffer, buffer + bufferSize, 0);

    int res = hid_read(deviceHandle, buffer, bufferSize);
    if (res < 0) {
#ifdef _WIN32
        std::wstring ws(hid_error(deviceHandle));
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        std::cerr << "Error reading HID device: " << conv.to_bytes(ws) << "\n";
#else
        std::cerr << "Error reading HID device: " << hid_error(deviceHandle) << "\n";
#endif
        return false;
    }

    // res == 0 won't happen in blocking mode unless timeout or disconnect
    return res > 0;
}

/**
 * @brief Parses data from buffer into Utils::MozaState
 * @param buffer - raw HID data
 * @return struct containing the parsed data
 */
 /* bytes:
  *  0    ?state?
  *  1, 2 wheel
  *  3, 4 clutchCombined
  *  5, 6 throttle
  *  7, 8 clutchRight
  *  9,10 clutchLeft
  * 11,12 brake
  * 13,14 ?handbrake?
  * 15,16 clutch
  * 17-32 dPad, buttons
  * 33    ?shifter & buttonHandbrake?
  * 34-41 steering velocity & accel
  */
Utils::MozaState MozaReader::parseReport(const unsigned char *buffer) {

    Utils::MozaState state{};

    /*printf("\rbuffer: ");
    for (size_t i = 0; i < 42; i++) {
        printf("%02X ", buffer[i]);
    }*/

    state.wheel = static_cast<int16_t>((buffer[1] | (buffer[2] << 8)) ^ 0x8000);
    state.clutchCombined = static_cast<int16_t>((buffer[3] | (buffer[4] << 8)));
    state.clutchRight = static_cast<int16_t>((buffer[7] | (buffer[8] << 8)));
    state.clutchLeft = static_cast<int16_t>((buffer[9] | (buffer[10] << 8)));
    state.throttle = static_cast<int16_t>((buffer[5] | (buffer[6] << 8)));
    state.clutch = static_cast<int16_t>((buffer[15] | (buffer[16] << 8)));
    state.brake = static_cast<int16_t>((buffer[11] | (buffer[12] << 8)));
    state.handbrake = static_cast<int16_t>((buffer[13] | (buffer[14] << 8)));
    state.buttonHandbrake = false;

    // --- Map buttons ---
    int buttonIndex = 0;
    for (int byteIdx = 17; byteIdx <= 32 && buttonIndex < 128; ++byteIdx) {
        for (int bit = 0; bit < 8 && buttonIndex < 128; ++bit) {
            if (byteIdx == 17 && bit < 3) continue;

            state.buttons[buttonIndex] = (buffer[byteIdx] & (1 << bit)) != 0;
            ++buttonIndex;
        }
    }
    DPad d = MozaReader::parse_dpad(buffer[17]);
    // map buttons to D-Pad in case D-Pad mode was activated in Moza Pit House
    state.buttons[5] |= d.up;
    state.buttons[6] |= d.right;
    state.buttons[7] |= d.down;
    state.buttons[8] |= d.left;
    // map buttons 15 & 16 to clutchLeft and clutchRight when fully pressed
    state.buttons[15] |= state.clutchLeft >= 32760;
    state.buttons[16] |= state.clutchRight >= 32760;

    return state;
}

DPad MozaReader::parse_dpad(unsigned char b) {
    b &= 0xF; // keep only 4 bits

    // bit n in each mask corresponds to direction active when b == n
    constexpr unsigned short UP_MASK    = (1 << 0x0) | (1 << 0x7) | (1 << 0x1);
    constexpr unsigned short DOWN_MASK  = (1 << 0x4) | (1 << 0x3) | (1 << 0x5);
    constexpr unsigned short LEFT_MASK  = (1 << 0x6) | (1 << 0x7) | (1 << 0x5);
    constexpr unsigned short RIGHT_MASK = (1 << 0x2) | (1 << 0x1) | (1 << 0x3);

    DPad d;

    unsigned short bit = 1 << b;

    d.up    = (UP_MASK    & bit) != 0;
    d.down  = (DOWN_MASK  & bit) != 0;
    d.left  = (LEFT_MASK  & bit) != 0;
    d.right = (RIGHT_MASK & bit) != 0;

    return d;
}