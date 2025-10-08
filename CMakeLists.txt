cmake_minimum_required(VERSION 3.16)

project(Moza2Joystick VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include vJoy headers
include_directories("C:/Program Files/vJoy/SDK/inc")

# Find hidapi via vcpkg
find_package(hidapi CONFIG REQUIRED)

# Collect all source files
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(Moza2Joystick ${SOURCES})

# Link libraries
target_link_libraries(Moza2Joystick
        PRIVATE hidapi::hidapi
        PRIVATE "C:/Program Files/vJoy/SDK/lib/amd64/vJoyInterface.lib"
)

# Optional: include project headers
target_include_directories(Moza2Joystick PRIVATE include)

# tests
# enable_testing()
# add_subdirectory(tests)


set(HIDAPI_DLL_PATH "D:/GitHub/vcpkg/installed/x64-windows/bin/hidapi.dll")
if(NOT EXISTS ${HIDAPI_DLL_PATH})
    set(HIDAPI_DLL_PATH "D:/GitHub/vcpkg/installed/x64-windows/debug/bin/hidapi.dll")
endif()
add_custom_command(TARGET Moza2Joystick POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${HIDAPI_DLL_PATH}"
        $<TARGET_FILE_DIR:Moza2Joystick>
)

