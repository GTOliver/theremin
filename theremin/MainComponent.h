#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "LeapCpp.h"
#include "CallbackListener.h"

class MainComponent : public juce::Component {
public:
    MainComponent();

    ~MainComponent() override;

    void paint(juce::Graphics &) override;

    void resized() override;

    void tracking_event_callback(LEAP_CONNECTION_MESSAGE event);

private:
    lpp::Connection connection_;
    std::shared_ptr<CallbackListener> listener_;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
