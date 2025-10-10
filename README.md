# Moza2Joystick

> A lightweight bridge that converts MOZA Racing wheel and pedal inputs into virtual joystick signals (via vJoy) 
> — enabling full controller compatibility even in games without native MOZA support.

---

## Overview

**Moza2Joystick** lets you use your MOZA hardware in _any_ game that supports standard game controllers.

It reads real-time input from your devices using either:

- [MOZA SDK](https://mozaracing.com/pages/sdk) — for maximum performance and official data access
- [hidapi](https://github.com/libusb/hidapi) — for a driver-independent fallback mode

All input is then forwarded to a **[vJoy virtual controller]((https://sourceforge.net/projects/vjoystick/))**, so your system sees it as a regular joystick or gamepad.

---

## Features

Dual input backends
- SDK mode: Uses the official MOZA SDK (`mozaAPI`)
- HID mode: Uses raw HID data via `hidapi`

vJoy output
- Emulates a fully functional joystick device
- Compatible with any title that supports DirectInput or XInput

Live input forwarding
- Wheel rotation, pedals, and buttons are mapped 1:1 to the vJoy device

Configurable via [config](./config.json)
- Switch between SDK and HID readers seamlessly
- Change button mappings

---

## Repository Structure

```
Moza2Joystick/
├── src/            # Core source code
├── include/        # Header files
├── examples/       # Sample setups or test utilities
├── docs/           # Documentation and protocol references
├── CMakeLists.txt  # Build system
├── config.json     # Configuration file
└── README.md
```

---

## Usage

1. Connect your Moza wheelbase and pedals.
2. Launch Moza2Joystick — it will automatically detect connected Moza hardware.
    - starts Moza Pit House automatically if set to Moza SDK in [config](./config.json)
3. The program updates the vJoy controller with values from you hardware.
4. Start your game and select the vJoy controller as input.

---

## License
See the [License](./LICENSE) file for details

(Open-source under a permissive license.)

---

## Credits
- Moza Racing — for their SDK
- vJoy Project — for the virtual joystick driver
- hidapi developers — for the cross-platform HID access library
