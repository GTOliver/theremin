add_library(LeapCpp
        STATIC
        src/LeapCpp.cpp
        src/ConnectionImpl.h
        src/ConnectionImpl.cpp
        src/Exception.h
        src/Exception.cpp
        )

target_include_directories(LeapCpp PUBLIC include)

target_link_libraries(LeapCpp PUBLIC LeapSDK::LeapC)
