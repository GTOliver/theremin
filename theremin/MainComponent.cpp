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
    const auto area = getLocalBounds();
    message_box_.setBounds(area.reduced(8));
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
