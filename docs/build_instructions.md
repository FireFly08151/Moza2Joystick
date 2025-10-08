## Building Moza2Joystick (Windows x64)

### Dependencies
- Visual Studio 2022 (x64 toolchain)
- vJoy SDK (x64)
- hidapi via vcpkg

### Build steps
1. Clone vcpkg and integrate: `vcpkg integrate install`
2. Install hidapi: `vcpkg install hidapi:x64-windows`
3. Copy required DLLs after build:
- `vJoyInterface.dll` &rarr; from `C:/Program Files/vJoy/x64/`
- `hidapi.dll` &rarr; from `vcpkg/installed/x64-windows/bin`

### Testing
Run `/examples/basic_connection_test/` to confirm both APIs work.
