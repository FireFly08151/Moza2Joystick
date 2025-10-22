cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

# ---------------------------------------------------------------
# Project Configuration
# ---------------------------------------------------------------
project(Moza2Joystick
        VERSION 0.1.0
        DESCRIPTION "Map Moza wheel input to virtual joystick via vJoy and MOZA SDK or hidapi"
        LANGUAGES CXX
)

# ---------------------------------------------------------------
# Configure output directories per build type
# ---------------------------------------------------------------
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
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_definitions(UNICODE _UNICODE)

message(STATUS "Building ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# ---------------------------------------------------------------
# Dependencies
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

# Find nlohmann/json.hpp (via vcpkg or system package)
find_package(nlohmann_json CONFIG REQUIRED)

# ---- Qt + OpenSSL + RS21 SDK (for API test) ----
if(DEFINED ENV{VCPKG_INSTALLED_DIR})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{VCPKG_INSTALLED_DIR}/$ENV{VCPKG_TARGET_TRIPLET}")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.3/msvc2022_64")  # adjust path
set(CMAKE_AUTOMOC ON)
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Widgets)

find_package(OpenSSL REQUIRED)

# RS21 SDK path
set(RS21_sdk "C:/Program Files (x86)/MOZA_SDK/1.0.1.6/MSVC2022-64")

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
        "${RS21_sdk}/include"
)

# ---------------------------------------------------------------
# Source Files
# ---------------------------------------------------------------
file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# ---------------------------------------------------------------
# Main Executable (Moza2Joystick)
# ---------------------------------------------------------------
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

link_directories("C:/Program Files (x86)/MOZA_SDK/1.0.1.6/MSVC2022-64/lib")

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        hidapi::hidapi
        "${VJOY_SDK_DIR}/lib/amd64/vJoyInterface.lib"
        dinput8
        dxguid
        user32
        "C:/Program Files (x86)/MOZA_SDK/1.0.1.6/MSVC2022-64/lib/MOZA_SDK.lib"
        nlohmann_json::nlohmann_json
)

# ---------------------------------------------------------------
# Basic Connection Executable
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
# API Test Executable (Qt + OpenSSL + RS21 SDK)
# ---------------------------------------------------------------
add_executable(APITest examples/moza_sdk_test/sdk_api_test.cc)

target_include_directories(APITest
        PUBLIC
        Qt${QT_VERSION_MAJOR}::Test
        ${RS21_sdk}/include
)

target_link_libraries(APITest
        PUBLIC
        Qt${QT_VERSION_MAJOR}::Test
        Qt::Widgets
        OpenSSL::SSL
        ${RS21_sdk}/lib/MOZA_SDK.lib
)

enable_testing()
add_test(NAME sdk_api_test COMMAND $<TARGET_FILE:APITest>)

# ---------------------------------------------------------------
# Post-Build: Copy DLLs (hidapi + vJoy + MOZA_SDK)
# ---------------------------------------------------------------
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

set(MOZA_SDK_DLL_PATH
        "${RS21_sdk}/bin/MOZA_SDK.dll"
        CACHE FILEPATH "Path to MOZA_SDK.dll"
)

set(QT_BIN_DIR "C:/Qt/6.9.3/msvc2022_64/bin" CACHE PATH "Qt bin directory")
set(WINDEPLOYQT_EXE "${QT_BIN_DIR}/windeployqt.exe")

set(CONFIG_FILE "${CMAKE_SOURCE_DIR}/config.json")

# Define which DLLs each target needs
set(HIDAPI_TARGETS ${PROJECT_NAME} Moza2Joystick APITest basic_connection_test)
set(VJOY_TARGETS ${PROJECT_NAME} Moza2Joystick basic_connection_test)
set(MOZA_TARGETS Moza2Joystick APITest)
set(QT_TARGETS APITest)
set(CONFIG_TARGETS "${PROJECT_NAME}" "Moza2Joystick")

foreach(target ${CONFIG_TARGETS})
    if(TARGET ${target} AND EXISTS "${CONFIG_FILE}")
        set(copy_target_name "copy_config_${target}")

        if(NOT TARGET ${copy_target_name})
            add_custom_target(${copy_target_name} ALL
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${CONFIG_FILE}"
                    "$<TARGET_FILE_DIR:${target}>/config.json"
                    COMMENT "Copying config.json for ${target} -> $<TARGET_FILE_DIR:${target}>"
                    VERBATIM
            )
        endif()

        add_dependencies(${target} ${copy_target_name})
    endif()
endforeach()

foreach(target ${PROJECT_NAME} Moza2Joystick APITest basic_connection_test)

    if(EXISTS "${HIDAPI_DLL_PATH}" AND target IN_LIST HIDAPI_TARGETS)
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${HIDAPI_DLL_PATH}" $<TARGET_FILE_DIR:${target}>
                COMMENT "Copying hidapi.dll for ${target}"
        )
    endif()

    if(EXISTS "${VJOYINTERFACE_DLL_PATH}" AND target IN_LIST VJOY_TARGETS)
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${VJOYINTERFACE_DLL_PATH}" $<TARGET_FILE_DIR:${target}>
                COMMENT "Copying vJoyInterface.dll for ${target}"
        )
    endif()

    if(EXISTS "${MOZA_SDK_DLL_PATH}" AND target IN_LIST MOZA_TARGETS)
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MOZA_SDK_DLL_PATH}" $<TARGET_FILE_DIR:${target}>
                COMMENT "Copying MOZA_SDK.dll for ${target}"
        )
    endif()

    if(EXISTS "${WINDEPLOYQT_EXE}" AND target IN_LIST QT_TARGETS)
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND "${WINDEPLOYQT_EXE}" --dir "$<TARGET_FILE_DIR:${target}>" "$<TARGET_FILE:${target}>"
                COMMENT "Running windeployqt to deploy Qt runtime for ${target}"
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
message(STATUS "RS21 SDK Dir:   ${RS21_sdk}")
message(STATUS "vJoyInterface:  ${VJOYINTERFACE_DLL_PATH}")
message(STATUS "hidapi.dll:     ${HIDAPI_DLL_PATH}")
message(STATUS "MOZA_SDK.dll:   ${MOZA_SDK_DLL_PATH}")
message(STATUS "=================================================")
message(STATUS "")
