#include "MainComponent.h"
#include "LeapC.h"
#include "LeapCpp.h"

MainComponent::MainComponent()
        : connection_(),
          listener_(std::make_unique<CallbackListener>(
                  [this](LEAP_CONNECTION_MESSAGE event) { tracking_event_callback(event); })) {
    connection_.add_listener(listener_);
    connection_.open();
    setSize(600, 400);
}

MainComponent::~MainComponent() {
    // We can safely call these regardless of whether the connection is open or the listener has been added.
    connection_.close();
    connection_.remove_listener(listener_);
}


void MainComponent::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setFont(juce::Font(16.0f));
    g.setColour(juce::Colours::white);
    juce::String my_str;
    my_str << LeapGetNow() << " " << lpp::get_now();
    g.drawText(my_str, getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized() {
}

void MainComponent::tracking_event_callback(LEAP_CONNECTION_MESSAGE event) {
    juce::ignoreUnused(event);
    juce::String my_str;
    my_str << "Got event: " << LeapGetNow() << "\n";
    juce::Logger::getCurrentLogger()->writeToLog(my_str);
}
