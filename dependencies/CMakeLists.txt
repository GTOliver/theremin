include(FetchContent)

message(STATUS "Fetching JUCE")
FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 6.1.6
        GIT_SHALLOW TRUE
        )
FetchContent_MakeAvailable(JUCE)

message(STATUS "Fetching readerwriterqueue")
FetchContent_Declare(
        readerwriterqueue
        GIT_REPOSITORY https://github.com/cameron314/readerwriterqueue
        GIT_TAG v1.0.6
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(readerwriterqueue)
