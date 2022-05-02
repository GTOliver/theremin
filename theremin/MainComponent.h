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
    void on_freq_slider_changed(juce::Slider* slider_ptr);

    void on_freq_range_slider_changed();

    void on_volume_slider_changed();

    void on_volume_range_slider_changed(juce::Slider* slider_ptr);

    void on_freq_scaling_changed();

    void on_snapping_changed();

    void update_ui(ThereMessage message);

    void tracking_callback(TrackingFrame frame);

    juce::Label live_level_label_;
    juce::Slider live_level_slider_;

    juce::Label live_frequency_label_;
    juce::Slider live_frequency_slider_;

    juce::Label volume_label_;
    juce::Slider volume_slider_;

    juce::Label volume_min_distance_label_;
    juce::Slider volume_min_distance_slider_;
    juce::Label volume_max_distance_label_;
    juce::Slider volume_max_distance_slider_;

    juce::Label freq_min_label_;
    juce::Slider freq_min_slider_;
    juce::Label freq_max_label_;
    juce::Slider freq_max_slider_;

    juce::Label freq_distance_label_;
    juce::Slider freq_distance_slider_;

    juce::Label freq_scaling_label_;
    juce::ComboBox freq_scaling_box_;

    juce::Label snapping_label_;
    juce::ComboBox snapping_box_;

    MessageQueue messages_;
    HandTracker tracker_;
    FrameProcessor frame_processor_;
    AudioProcessor audio_processor_;

    std::optional<ThereMessage> latest_message_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
