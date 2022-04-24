#pragma once

#include <atomic>

#include <juce_gui_extra/juce_gui_extra.h>

#include "CallbackListener.h"
#include "LeapCpp.h"

class MainComponent: public juce::Component, private juce::Timer
{
public:
    MainComponent();

    ~MainComponent() override;

    void resized() override;

    void timerCallback() override;

    void tracking_event_callback(LEAP_CONNECTION_MESSAGE event);

    void log_message(const juce::String& msg);

private:
    lpp::Connection connection_;
    std::shared_ptr<CallbackListener> listener_;

    juce::TextEditor message_box_;

    std::atomic<lpp::Timestamp> latest_timestamp_;
    static_assert(decltype(latest_timestamp_)::is_always_lock_free);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
