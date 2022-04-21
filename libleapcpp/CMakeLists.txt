add_library(LeapCpp
        STATIC
        src/LeapCpp.cpp
        )

target_include_directories(LeapCpp PUBLIC include)

target_link_libraries(LeapCpp PUBLIC LeapSDK::LeapC)
