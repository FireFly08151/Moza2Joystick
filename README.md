# Moza2Joystick

> A lightweight HID-to-vJoy bridge that converts **MOZA wheel and pedal inputs** into **virtual joystick signals**, enabling full controller compatibility in games that don’t natively support Moza hardware.

---

## Overview

**Moza2Joystick** directly interfaces with **MOZA Racing hardware** over the MOZA SDK.

It reads the values from the [SDK](https://mozaracing.com/pages/sdk) and forwards them to a **virtual joystick device** provided by [**vJoy**](http://vjoystick.sourceforge.net/).

This makes it possible to use your Moza gear in:
- Games without native Moza support (e.g. older sims, rally titles, indie racers)
- Emulators and generic joystick-input applications

---

## Features

- **MOZA SDK** — direct data from the SDK
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

## Usage

1. Connect your Moza wheelbase and pedals.
2. Launch Moza2Joystick — it will automatically detect connected Moza hardware.
3. The program updates the vJoy controller with values from you hardware.
4. Start your game and select the vJoy controller as input.

---

## License
See the [License](./LICENSE) file for details

(Open-source under a permissive license.)

---

## Credits
- Moza Racing — for hardware
- vJoy communities — for virtual device support
