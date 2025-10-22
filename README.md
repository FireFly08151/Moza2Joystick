# Moza2Joystick

> A lightweight bridge that converts MOZA Racing wheel and pedal inputs into virtual joystick signals (via vJoy) 
> — enabling full controller compatibility even in games without native MOZA support.

---

## Overview

**Moza2Joystick** lets you use your MOZA hardware in _any_ game that supports standard game controllers.

It reads real-time input from your devices using either:

- [MOZA SDK](https://mozaracing.com/pages/sdk) - for official data access
- [hidapi](https://github.com/libusb/hidapi) - for independence from Moza Pit House

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

1. Connect your base to the computer and the remaining devices such as pedals and handbrake to the base.
2. Launch Moza2Joystick — it will automatically detect connected Moza hardware.
   - starts Moza Pit House automatically if set to Moza SDK in [config](./config.json)
3. The program updates the vJoy controller with values from you hardware.
4. Start your game and select the vJoy controller as input.
   - if you can't change the mappings in-game just remap the controls in the [config](./config.json)-file

---

## Important Notes
- Your hardware needs to be calibrated using [MOZA Pit House](https://mozaracing.com/pages/pit-house)
- The shifter won't work (<mark>ToDo</mark>: I don't own one so i haven't implemented any logic yet)
### hidapi-Mode
- The handbrake might not be working as expected due to me not owning one (<mark>ToDo</mark>: make a small python-script to capture input from someone with a handbrake)
### SDK-Mode
- Make sure to set `Stick Mode` to `Button` in Moza Pit House (<mark>ToDo</mark>: or just enable automatic switch in the [config](./config.json)-file)
   > Joysticks in D-Pad-Mode don't get exposed in the SDK
- Set `Dual Clutch Paddle Mode` either to `Axis-Combine` or `Axis-Split` (<mark>ToDo</mark>: or just enable automatic selection in the [config](./config.json)-file)
   > `Axis-Combine` does not enable buttons 15 & 16 (shifter buttons), `Axis-Split` does.

---

## License
See the [License](./LICENSE) file for details

(Open-source under a permissive license.)

---

## Credits
- hidapi developers — for the cross-platform HID access library
- Moza Racing — for their SDK
- vJoy Project — for the virtual joystick driver
