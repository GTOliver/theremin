cmake_minimum_required(VERSION 3.21)
project(hand_tracked_theremin)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(LEAP_SDK_INSTALLED_DIR "$ENV{ProgramFiles}/Ultraleap")

find_package(LeapSDK
        REQUIRED
        PATHS
        "${LEAP_SDK_INSTALLED_DIR}")

add_subdirectory(dependencies)
add_subdirectory(libleapcpp)
add_subdirectory(theremin)
