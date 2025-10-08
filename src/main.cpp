#include <windows.h>
#include "hidapi.h"
#include "public.h"
#include "vJoyInterface.h"
#include <iostream>

int main() {
    // Test vJoy
    if (vJoyEnabled())
        std::cout << "vJoy driver detected!" << std::endl;
    else
        std::cout << "vJoy driver NOT detected!" << std::endl;

    // Test HID
    hid_init();
    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(0x0000, 0x0000); // list all
    cur_dev = devs;
    while (cur_dev) {
        std::wcout << L"Device: " << cur_dev->manufacturer_string
                   << L" | Product: " << cur_dev->product_string
                   << L" | VID: " << std::hex << cur_dev->vendor_id
                   << L" PID: " << cur_dev->product_id << std::endl;
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
    hid_exit();

    return 0;
}
