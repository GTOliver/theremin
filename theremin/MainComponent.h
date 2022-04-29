#pragma once

#include <atomic>

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include "CallbackListener.h"
#include "LeapCpp.h"

#include "LevelCalculator.h"
#include "PhaseChangeCalculator.h"

class MainComponent: public juce::AudioAppComponent, private juce::Timer
{
public:
    MainComponent();

    ~MainComponent() override;

    void resized() override;

    void timerCallback() override;

    void prepareToPlay(int samples_per_block, double sample_rate) override;

    void releaseResources() override;

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& audio_data) override;

    void tracking_event_callback(LEAP_CONNECTION_MESSAGE event);

    void log_message(const juce::String& msg);

private:
    lpp::Connection connection_;
    std::shared_ptr<CallbackListener> listener_;

    juce::TextEditor message_box_;

    LevelCalculator level_calculator_;
    PhaseChangeCalculator phase_change_calculator_;

    std::atomic<lpp::Timestamp> latest_timestamp_;
    static_assert(decltype(latest_timestamp_)::is_always_lock_free);

    // Note: All 3 of these should be a single thing which is passed to
    // the audio thread atomically, however it doesn't really matter
    // because it doesn't matter if any one of these is out of sync
    // with the others - as long as "both_hands_present" is updated
    // last.
    std::atomic<double> rh_z_;
    std::atomic<double> lh_y_;
    std::atomic<bool> both_hands_present_;

    std::atomic<double> phase_change_per_sample_;
    std::atomic<double> level_;

    double phase_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
