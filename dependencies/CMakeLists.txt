include(FetchContent)

FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 6.1.6
        GIT_SHALLOW TRUE
        )
FetchContent_MakeAvailable(JUCE)
