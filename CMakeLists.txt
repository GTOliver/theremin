cmake_minimum_required(VERSION 3.21)
project(hand_tracked_theremin)

set(CMAKE_CXX_STANDARD 17)

set(LEAP_SDK_INSTALLED_DIR "$ENV{ProgramFiles}/Ultraleap")

find_package(LeapSDK
        REQUIRED
        PATHS
        "${LEAP_SDK_INSTALLED_DIR}")

get_target_property(
        LEAPC_IMPORTED_CONFIG
        LeapSDK::LeapC
        IMPORTED_CONFIGURATIONS
)

get_target_property(
        LEAPC_SHARED_LIB_PATH
        LeapSDK::LeapC
        IMPORTED_LOCATION_${LEAPC_IMPORTED_CONFIG}
)

add_subdirectory(dependencies)
add_subdirectory(theremin)
