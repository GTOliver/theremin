#include "juce_events/juce_events.h"

#include "MainComponent.h"

MainComponent::MainComponent()
        : message_box_(),
          tracker_{},
          audio_processor_{},
          messages_{},
          latest_message_{}
{
    tracker_.set_callback([this](TrackingFrame frame) { tracking_callback(frame); });

    addAndMakeVisible(message_box_);
    message_box_.setMultiLine(true);
    message_box_.setReturnKeyStartsNewLine(true);
    message_box_.setReadOnly(true);
    message_box_.setScrollbarsShown(true);
    message_box_.setCaretVisible(false);
    message_box_.setPopupMenuEnabled(true);

    addAndMakeVisible(level_label_);
    level_label_.setText("Level", juce::dontSendNotification);

    addAndMakeVisible(level_slider_);
    level_slider_.setRange(0.0, 0.2);

    addAndMakeVisible(frequency_label_);
    frequency_label_.setText("Frequency", juce::dontSendNotification);

    addAndMakeVisible(frequency_slider_);
    frequency_slider_.setRange(500.0, 2000.0);

    setSize(600, 400);

    setAudioChannels(0, 2);
    startTimerHz(30);
}

MainComponent::~MainComponent()
{
    tracker_.clear_callback();
    shutdownAudio();
}

void MainComponent::resized()
{
    auto half_width = getWidth() / 2;

    auto slider_bounds = getLocalBounds().withWidth(half_width).reduced(10);

    level_label_.setBounds(slider_bounds.getX(), 10, slider_bounds.getWidth(), 20);
    level_slider_.setBounds(slider_bounds.getX(), 40, slider_bounds.getWidth(), 20);
    frequency_label_.setBounds(slider_bounds.getX(), 70, slider_bounds.getWidth(), 20);
    frequency_slider_.setBounds(slider_bounds.getX(), 100, slider_bounds.getWidth(), 20);

    message_box_.setBounds(getLocalBounds().withWidth(half_width).withX(half_width).reduced(10));
}

void MainComponent::prepareToPlay(int samples_per_block, double sample_rate)
{
    audio_processor_.prepare(sample_rate);

    juce::String msg;
    msg << "Preparing to play:" << juce::newLine;
    msg << "\tSample Rate: " << sample_rate << juce::newLine;
    msg << "\tBuffer Size: " << samples_per_block << juce::newLine;
    log_message(msg);
}

void MainComponent::releaseResources()
{
    juce::String msg;
    msg << "Releasing resources..." << juce::newLine;
    log_message(msg);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& audio_data)
{
    while (messages_.has_next()) {
        const auto msg = messages_.next();
        audio_processor_.process(msg);
    }
    audio_processor_.get_next_audio_block(audio_data);
}

void MainComponent::timerCallback()
{
    std::optional<ThereMessage> message = std::nullopt;
    latest_message_.swap(message);

    if (message.has_value()) {
        level_slider_.setValue(message->level, juce::dontSendNotification);
        frequency_slider_.setValue(message->frequency, juce::dontSendNotification);
        juce::String my_str;
        my_str << message->level << " : " << message->frequency;
        log_message(my_str);
    }
}

void MainComponent::log_message(const juce::String& msg)
{
    message_box_.moveCaretToEnd();
    message_box_.insertTextAtCaret(msg + juce::newLine);
}

void MainComponent::update_ui(ThereMessage message)
{
    latest_message_ = message;
}

void MainComponent::tracking_callback(TrackingFrame frame)
{
    const auto message = frame_processor_.process(frame);
    if (message.has_value()) {
        messages_.send(message.value());
        juce::MessageManager::callAsync([this, message]() { update_ui(message.value()); });
    }
}
