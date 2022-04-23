#include "MainComponent.h"
#include "LeapC.h"
#include "LeapCpp.h"

MainComponent::MainComponent()
        : connection_(),
          listener_(std::make_unique<CallbackListener>(
                  [this](LEAP_CONNECTION_MESSAGE event) { tracking_event_callback(event); })),
          message_box_(),
          latest_timestamp_{0} {
    connection_.add_listener(listener_);
    connection_.open();

    addAndMakeVisible(message_box_);
    message_box_.setMultiLine(true);
    message_box_.setReturnKeyStartsNewLine(true);
    message_box_.setReadOnly(true);
    message_box_.setScrollbarsShown(true);
    message_box_.setCaretVisible(false);
    message_box_.setPopupMenuEnabled(true);

    setSize(600, 400);

    startTimerHz(30);
}

MainComponent::~MainComponent() {
    // We can safely call these regardless of whether the connection is open or the listener has been added.
    connection_.close();
    connection_.remove_listener(listener_);
}

void MainComponent::resized() {
    const auto area = getLocalBounds();
    message_box_.setBounds(area.reduced(8));
}

void MainComponent::timerCallback() {
    auto result = latest_timestamp_.exchange(0);
    if (result) {
        juce::String my_str;
        my_str << result;
        log_message(my_str);
    }
}

void MainComponent::tracking_event_callback([[maybe_unused]] LEAP_CONNECTION_MESSAGE event) {
    latest_timestamp_.store(lpp::get_now());
}

void MainComponent::log_message(const juce::String &msg) {
    message_box_.moveCaretToEnd();
    message_box_.insertTextAtCaret(msg + juce::newLine);
}
