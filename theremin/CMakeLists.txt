include(CopyLeapLibrary)

juce_add_gui_app(Theremin
        VERSION 0.0.1
        PRODUCT_NAME "Theremin")

target_sources(Theremin
        PRIVATE
        main.cpp
        AudioProcessor.h
        AudioProcessor.cpp
        CallbackListener.h
        MainComponent.h
        MainComponent.cpp
        LevelCalculator.h
        LevelCalculator.cpp
        MessageQueue.h
        MessageQueue.cpp
        TrackingFrame.h
        HandTracker.h
        HandTracker.cpp
        FrameProcessor.h
        FrameProcessor.cpp
        FrequencyCalculator.h
        FrequencyCalculator.cpp
        )

target_compile_definitions(Theremin
        PRIVATE
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:Theremin,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:Theremin,JUCE_VERSION>")

target_link_libraries(Theremin
        PRIVATE
        LeapCpp
        readerwriterqueue
        juce::juce_audio_utils
        juce::juce_gui_extra
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

copy_leap_library(Theremin)
