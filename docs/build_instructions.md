# Building Moza2Joystick (Windows x64)

## Dependencies
| Dependency              | Source                                                            | Notes                                                          |
|-------------------------|-------------------------------------------------------------------|----------------------------------------------------------------|
| **Visual Studio 2022**  | System toolchain                                                  | Use MSVC x64 build tools                                       |
| **CMake ≥ 3.21**        | [cmake.org](https://cmake.org/download/)                          | Required for configuration and build                           |
| **vcpkg**               | [github.com/microsoft/vcpkg](https://github.com/microsoft/vcpkg)  | Used for dependency management                                 |
| **vJoy SDK (x64)**      | [vJoy Official Site](https://sourceforge.net/projects/vjoystick/) | Provides `vJoyInterface.lib` and `.dll`                        |
| **hidapi**              | via vcpkg (`vcpkg install hidapi:x64-windows`)                    | USB HID access                                                 |
| **Qt (Base + Tools)**   | via vcpkg (`vcpkg install qtbase qttools:x64-windows`)            | Needed for `APITest` target                                    |
| **OpenSSL**             | via vcpkg (`vcpkg install openssl:x64-windows`)                   | Needed by `APITest` target                                     |
| **RS21 SDK (MOZA SDK)** | Local install                                                     | Example: `C:/Program Files (x86)/MOZA_SDK/1.0.0.7/MSVC2019-64` |

---

## Build steps
### 1. Clone the repository
```bash
git clone https://github.com/FireFly08151/Moza2Joystick.git
cd Moza2Joystick
```
### 2. Set up vcpkg
```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
cd ..
vcpkg integrate install
vcpkg install hidapi openssl qtbase qttools:x64-windows
```
### 3. Configure CMake
From the project root:
```bash
cmake -S . -B builds/debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake -S . -B builds/release -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake
```
> This will generate Debug builds in `builds/debug` and Release builds in `builds/release`.
If using CLion, set the CMake build directory in your profile to match these folders.

### 4. Build
```bash
cmake --build builds/debug --config Debug
cmake --build builds/release --config Release
```
### 5. Copy required DLLs
(if not automatically copied by CMake post-build commands)

| DLL               | Source                                                                         |
|-------------------|--------------------------------------------------------------------------------|
| vJoyInterface.dll | From your vJoy SDK: `C:/Program Files/vJoy/SDK/lib/amd64/`                     |
| hidapi.dll        | From vcpkg: `vcpkg/installed/x64-windows/bin/`                                 |
| MOZA_SDK.dll      | From your vJoy SDK: `C:/Program Files (x86)/MOZA_SDK/1.0.1.6/MSVC2022-64/bin/` |

---

## Notes for CLion Users

- By default, CLion creates its own build folders (`cmake-build-debug-visual-studio`).
- To match the `/builds/debug` and `/builds/release` structure:
  1. Go to File &rarr; Settings &rarr; Build, Execution, Deployment &rarr; CMake
  2. For the Debug profile, set Build directory &rarr; `builds/debug`
  3. For the Release profile, set Build directory &rarr; `builds/release`
- Reload the project after changing these settings.
