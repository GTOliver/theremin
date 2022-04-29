#include "MainComponent.h"
#include "LeapC.h"
#include "LeapCpp.h"

#include <iostream>

#include <math.h>

MainComponent::MainComponent()
        : connection_(),
          listener_(std::make_unique<CallbackListener>(
                  [this](LEAP_CONNECTION_MESSAGE event) { tracking_event_callback(event); })),
          message_box_(),
          latest_timestamp_{0},
          rh_z_{0},
          lh_y_{0},
          both_hands_present_{false},
          phase_change_per_sample_{0},
          level_{0},
          phase_{0}
{
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

    startTimerHz(1);

    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    // We can safely call these regardless of whether the connection is open or the listener has been added.
    connection_.close();
    connection_.remove_listener(listener_);

    shutdownAudio();
}

void MainComponent::resized()
{
    const auto area = getLocalBounds();
    message_box_.setBounds(area.reduced(8));
}

void MainComponent::timerCallback()
{
    auto result = latest_timestamp_.exchange(0);

    auto rh = rh_z_.load();
    auto lh = lh_y_.load();
    auto present = both_hands_present_.load();

    juce::String present_str = present ? "on" : "off";

    if (result) {
        juce::String my_str;
        my_str << result << " " << present_str << ": " << lh << " " << rh;
        log_message(my_str);
    }
}

void MainComponent::prepareToPlay(int samples_per_block, double sample_rate)
{
    phase_change_calculator_.prepare(sample_rate);

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
    auto muted = !both_hands_present_.load();
    float level = muted ? 0.0f : static_cast<float>(level_.load());
    auto phase_change_per_sample = phase_change_per_sample_.load();

    auto* leftBuffer = audio_data.buffer->getWritePointer(0, audio_data.startSample);
    auto* rightBuffer = audio_data.buffer->getWritePointer(1, audio_data.startSample);

    for (auto sample = 0; sample < audio_data.numSamples; ++sample) {
        auto current_value = static_cast<float>(std::sin(phase_));
        phase_ += phase_change_per_sample;
        leftBuffer[sample] = current_value * level;
        rightBuffer[sample] = current_value * level;
    }

    double m_pi = 3.14159265358979323846;
    phase_ = std::fmod(phase_, 2.0 * m_pi);
}

void MainComponent::tracking_event_callback([[maybe_unused]] LEAP_CONNECTION_MESSAGE event)
{
    const double MM_TO_M = 0.001;
    if (event.type != eLeapEventType_Tracking)
        return;

    const auto extract_joint = [](const LEAP_HAND& hand) -> LEAP_VECTOR {
        return hand.palm.position;
    };

    // NOTE: For now we just write all components straight to the MainComponent.
    // This will be replaced with a separate processing step where we determine
    // which _events_ to generate and send to the audio thread.
    //
    // This is probably going to be MIDI events that would be added to a LF-queue
    // and processed separately.
    //
    // Other information would need to be sent somewhere for the UI thread to
    // read from.

    const auto* frame = event.tracking_event;
    latest_timestamp_.store(lpp::get_now());
    for (unsigned int i = 0; i < frame->nHands; ++i) {
        const auto hand = frame->pHands[i];
        if (hand.type == eLeapHandType_Left) {
            lh_y_.store(extract_joint(hand).y * MM_TO_M);
        } else {
            rh_z_.store(extract_joint(hand).z * MM_TO_M * -1.0f);
        }
    }
    both_hands_present_.store(frame->nHands == 2);

    if (frame->nHands == 2) {
        phase_change_per_sample_.store(phase_change_calculator_.calculate(rh_z_.load()));
        level_.store(level_calculator_.calculate(lh_y_.load()));
    }
}

void MainComponent::log_message(const juce::String& msg)
{
    message_box_.moveCaretToEnd();
    message_box_.insertTextAtCaret(msg + juce::newLine);
}
