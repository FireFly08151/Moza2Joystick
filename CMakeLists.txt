cmake_minimum_required(VERSION 3.16)

project(Moza2Joystick VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(UNICODE _UNICODE)

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")


# vJoy SDK path
set(VJOY_SDK_DIR "C:/Program Files/vJoy/SDK")

# Include vJoy headers
include_directories(
        "C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/um"
        "C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/shared"
        "C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/ucrt"
        ${VJOY_SDK_DIR}/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Find hidapi via vcpkg
find_package(hidapi CONFIG REQUIRED)

# Collect all source files
file(GLOB SOURCES "src/*.cpp")

# Main executable
add_executable(Moza2Joystick ${SOURCES})
target_link_libraries(Moza2Joystick
        PRIVATE hidapi::hidapi
        PRIVATE "C:/Program Files/vJoy/SDK/lib/amd64/vJoyInterface.lib"
        dinput8
        dxguid
        user32
)
target_include_directories(Moza2Joystick PRIVATE include)

# Optional: build example programs separately
add_executable(basic_connection_test examples/basic_connection_test/main.cpp)
target_link_libraries(basic_connection_test
        PRIVATE hidapi::hidapi
        PRIVATE "C:/Program Files/vJoy/SDK/lib/amd64/vJoyInterface.lib"
)

# Copy hidapi.dll after build
set(HIDAPI_DLL_PATH "D:/GitHub/vcpkg/installed/x64-windows/bin/hidapi.dll")
if(NOT EXISTS ${HIDAPI_DLL_PATH})
    set(HIDAPI_DLL_PATH "D:/GitHub/vcpkg/installed/x64-windows/debug/bin/hidapi.dll")
endif()

add_custom_command(TARGET Moza2Joystick POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${HIDAPI_DLL_PATH}"
        $<TARGET_FILE_DIR:Moza2Joystick>
)

add_custom_command(TARGET basic_connection_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${HIDAPI_DLL_PATH}"
        $<TARGET_FILE_DIR:basic_connection_test>
)

# Copy vJoyInterface.dll after build
set(VJOYINTERFACE_DLL_PATH "C:/Program Files/vJoy/SDK/lib/amd64/vJoyInterface.dll")
if(NOT EXISTS ${VJOYINTERFACE_DLL_PATH})
    set(VJOYINTERFACE_DLL_PATH "C:/Program Files/vJoy/SDK/lib/amd64/vJoyInterface.dll")
endif()

add_custom_command(TARGET Moza2Joystick POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VJOYINTERFACE_DLL_PATH}"
        $<TARGET_FILE_DIR:Moza2Joystick>
)

add_custom_command(TARGET basic_connection_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VJOYINTERFACE_DLL_PATH}"
        $<TARGET_FILE_DIR:basic_connection_test>
)