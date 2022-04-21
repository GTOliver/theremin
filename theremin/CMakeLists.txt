juce_add_gui_app(Theremin
        VERSION 0.0.1
        PRODUCT_NAME "Theremin")

target_sources(Theremin
        PRIVATE
        main.cpp
        MainComponent.h
        MainComponent.cpp)

target_compile_definitions(Theremin
        PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:Theremin,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:Theremin,JUCE_VERSION>")

target_link_libraries(Theremin
        PRIVATE
        LeapCpp
        juce::juce_gui_extra
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)


add_custom_command(
        TARGET
        Theremin
        POST_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy
        ${LEAPC_SHARED_LIB_PATH}
        $<TARGET_FILE_DIR:Theremin>)
