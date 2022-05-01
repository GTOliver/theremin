#pragma once

#include <atomic>
#include <optional>

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include "CallbackListener.h"

#include "LevelCalculator.h"

#include "AudioProcessor.h"
#include "FrameProcessor.h"
#include "HandTracker.h"
#include "MessageQueue.h"
#include "TrackingFrame.h"

class MainComponent: public juce::AudioAppComponent, private juce::Timer
{
public:
    MainComponent();

    ~MainComponent() override;

    void resized() override;

    void prepareToPlay(int samples_per_block, double sample_rate) override;

    void releaseResources() override;

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& audio_data) override;

    void timerCallback() override;

private:
    void log_message(const juce::String& msg);

    void update_ui(ThereMessage message);

    void tracking_callback(TrackingFrame frame);

    juce::TextEditor message_box_;

    MessageQueue messages_;
    HandTracker tracker_;
    FrameProcessor frame_processor_;
    AudioProcessor audio_processor_;

    std::optional<ThereMessage> latest_message_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
