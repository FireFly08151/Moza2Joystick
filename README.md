# Moza2Joystick

> A lightweight HID-to-vJoy bridge that converts **MOZA wheel and pedal inputs** into **virtual joystick signals**, enabling full controller compatibility in games that don’t natively support Moza hardware.

---

## Overview

**Moza2Joystick** directly interfaces with **MOZA Racing hardware** over **HID** (via [`hidapi`](https://github.com/libusb/hidapi)), bypassing the official *MOZA Pit House* software entirely.

It reads and parses the raw HID reports sent by the wheelbase and pedals, translates them into normalized axis and button values, and forwards them to a **virtual joystick device** provided by [**vJoy**](http://vjoystick.sourceforge.net/).

This makes it possible to use your Moza gear in:
- Games without native Moza support (e.g. older sims, rally titles, indie racers)
- Emulators and generic joystick-input applications

---

## Features

- **Direct HID communication** — no Pit House or SDK needed
- **vJoy integration** — presents as a standard joystick or controller

---

## Repository Structure

```
Moza2Joystick/
├── src/            # Core source code
├── include/        # Header files
├── config/         # Example configuration profiles
├── examples/       # Sample setups or test utilities
├── docs/           # Documentation and protocol references
├── CMakeLists.txt  # Build system
└── README.md
```

---

## Building from Source

```bash
git clone --branch main https://github.com/FireFly08151/Moza2Joystick.git
cd Moza2Joystick
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```
After building, ensure that vJoy is installed and the corresponding driver is active.

---

## Usage

1. Connect your Moza wheelbase and pedals.
2. Launch Moza2Joystick — it will automatically detect connected Moza hardware.
3. The program updates the vJoy controller with values from you hardware.
4. Start your game and select the vJoy controller as input.

---

## Development (dev branch)

- The dev branch introduces:
- Refactored Moza SDK input polling
- Early support for ViGEm output backend
- Improved calibration UI (planned)
- Logging and debug utilities

> These features are experimental — expect potential bugs and unstable behavior.

---

## License
See the [License](./LICENSE) file for details

(Open-source under a permissive license.)

---

## Credits
- Moza Racing — for hardware
- vJoy and ViGEm communities — for virtual device support
- Contributors and testers improving compatibility and usability
