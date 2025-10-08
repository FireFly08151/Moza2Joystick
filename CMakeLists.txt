cmake_minimum_required(VERSION 3.16)

# ---------------------------------------------------------------
# Project Configuration
# ---------------------------------------------------------------
project(Moza2Joystick
        VERSION 0.1.0
        DESCRIPTION "Map Moza wheel input to virtual joystick via vJoy"
        LANGUAGES CXX
)


# ---------------------------------------------------------------
# Configure output directories per build type
# ---------------------------------------------------------------
# Base output folder
set(BUILD_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/builds)

# Runtime (.exe, .dll)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${BUILD_OUTPUT_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_DIR}/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BUILD_OUTPUT_DIR}/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BUILD_OUTPUT_DIR}/release)

# Static libraries (.lib, .a)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${BUILD_OUTPUT_DIR}/debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_DIR}/release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BUILD_OUTPUT_DIR}/release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${BUILD_OUTPUT_DIR}/release)

# Shared libraries (.dll, .so, .dylib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${BUILD_OUTPUT_DIR}/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_DIR}/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BUILD_OUTPUT_DIR}/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${BUILD_OUTPUT_DIR}/release)


# ---------------------------------------------------------------
# C++ Standard and Compiler Settings
# ---------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions(UNICODE _UNICODE)

message(STATUS "Building ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# ---------------------------------------------------------------
# Options and Dependencies
# ---------------------------------------------------------------
# Allow user to specify the vJoy SDK root directory manually or via env var
set(VJOY_SDK_DIR
        "$ENV{VJOY_SDK_DIR}"
        CACHE PATH "Path to the vJoy SDK directory (e.g. C:/Program Files/vJoy/SDK)"
)

if(NOT VJOY_SDK_DIR)
    message(WARNING "VJOY_SDK_DIR not set. Please configure it manually.")
endif()

# Find hidapi (via vcpkg or system package)
find_package(hidapi CONFIG REQUIRED)

# ---------------------------------------------------------------
# Include Directories
# ---------------------------------------------------------------
set(WINDOWS_KITS_DIR
        "C:/Program Files (x86)/Windows Kits/10/Include"
        CACHE PATH "Base path for Windows SDK headers"
)

include_directories(
        "${WINDOWS_KITS_DIR}/10.0.26100.0/um"
        "${WINDOWS_KITS_DIR}/10.0.26100.0/shared"
        "${WINDOWS_KITS_DIR}/10.0.26100.0/ucrt"
        "${VJOY_SDK_DIR}/inc"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# ---------------------------------------------------------------
# Source Files
# ---------------------------------------------------------------
file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# ---------------------------------------------------------------
# Main Executable
# ---------------------------------------------------------------
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        hidapi::hidapi
        "${VJOY_SDK_DIR}/lib/amd64/vJoyInterface.lib"
        dinput8
        dxguid
        user32
)

# ---------------------------------------------------------------
# Example Executables
# ---------------------------------------------------------------
add_executable(basic_connection_test
        examples/basic_connection_test/main.cpp
)

target_link_libraries(basic_connection_test
        PRIVATE
        hidapi::hidapi
        "${VJOY_SDK_DIR}/lib/amd64/vJoyInterface.lib"
)

# ---------------------------------------------------------------
# Post-Build: Copy DLLs (hidapi + vJoy)
# ---------------------------------------------------------------
# Let users override via cache or environment
set(HIDAPI_DLL_PATH
        "$ENV{HIDAPI_DLL_PATH}"
        CACHE FILEPATH "Path to hidapi.dll"
)

if(NOT HIDAPI_DLL_PATH)
    set(HIDAPI_DLL_PATH "${VCPKG_INSTALLED_DIR}/x64-windows/bin/hidapi.dll")
endif()

set(VJOYINTERFACE_DLL_PATH
        "${VJOY_SDK_DIR}/lib/amd64/vJoyInterface.dll"
        CACHE FILEPATH "Path to vJoyInterface.dll"
)

foreach(target ${PROJECT_NAME} basic_connection_test)
    if(EXISTS "${HIDAPI_DLL_PATH}")
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${HIDAPI_DLL_PATH}" $<TARGET_FILE_DIR:${target}>
                COMMENT "Copying hidapi.dll for ${target}"
        )
    endif()

    if(EXISTS "${VJOYINTERFACE_DLL_PATH}")
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${VJOYINTERFACE_DLL_PATH}" $<TARGET_FILE_DIR:${target}>
                COMMENT "Copying vJoyInterface.dll for ${target}"
        )
    endif()
endforeach()

# ---------------------------------------------------------------
# Summary
# ---------------------------------------------------------------
message(STATUS "")
message(STATUS "========== Build Configuration Summary ==========")
message(STATUS "Project:        ${PROJECT_NAME}")
message(STATUS "Version:        ${PROJECT_VERSION}")
message(STATUS "Build Type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "vJoy SDK Dir:   ${VJOY_SDK_DIR}")
message(STATUS "vJoyInterface:  ${VJOYINTERFACE_DLL_PATH}")
message(STATUS "hidapi.dll:     ${HIDAPI_DLL_PATH}")
message(STATUS "=================================================")
message(STATUS "")
