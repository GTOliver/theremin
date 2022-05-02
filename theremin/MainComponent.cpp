#include "juce_events/juce_events.h"

#include "MainComponent.h"

MainComponent::MainComponent()
        : tracker_{},
          audio_processor_{},
          messages_{},
          latest_message_{}
{
    tracker_.set_callback([this](TrackingFrame frame) { tracking_callback(frame); });

    addAndMakeVisible(live_level_label_);
    live_level_label_.setText("Level", juce::dontSendNotification);
    addAndMakeVisible(live_level_slider_);
    live_level_slider_.setRange(0.0, 0.2);

    double freq_absolute_min = 0.0;
    double freq_absolute_max = 10000.0;
    double freq_initial_min = 440.0;
    double freq_initial_max = 1760.0;

    addAndMakeVisible(live_frequency_label_);
    live_frequency_label_.setText("Frequency", juce::dontSendNotification);
    addAndMakeVisible(live_frequency_slider_);
    live_frequency_slider_.setRange(freq_initial_min, freq_initial_max);
    live_frequency_slider_.setSkewFactorFromMidPoint(1000.0);

    addAndMakeVisible(volume_label_);
    volume_label_.setText("Volume", juce::dontSendNotification);
    addAndMakeVisible(volume_slider_);
    volume_slider_.setRange(0.0, 0.5);
    volume_slider_.onValueChange = [this]() { on_volume_slider_changed(); };
    volume_slider_.setValue(0.2);

    addAndMakeVisible(volume_min_distance_label_);
    volume_min_distance_label_.setText("Volume Min Distance (m)", juce::dontSendNotification);
    addAndMakeVisible(volume_min_distance_slider_);
    volume_min_distance_slider_.setRange(0.0, 0.5);
    volume_min_distance_slider_.onValueChange = [this, ptr = &volume_min_distance_slider_]() {
        on_volume_range_slider_changed(ptr);
    };
    volume_min_distance_slider_.setValue(0.15, juce::dontSendNotification);

    addAndMakeVisible(volume_max_distance_label_);
    volume_max_distance_label_.setText("Volume Max Distance (m)", juce::dontSendNotification);
    addAndMakeVisible(volume_max_distance_slider_);
    volume_max_distance_slider_.setRange(0.15, 1.0);
    volume_max_distance_slider_.onValueChange = [this, ptr = &volume_max_distance_slider_]() {
        on_volume_range_slider_changed(ptr);
    };
    volume_max_distance_slider_.setValue(0.5);

    addAndMakeVisible(freq_min_label_);
    freq_min_label_.setText("Min Frequency", juce::dontSendNotification);
    addAndMakeVisible(freq_min_slider_);
    freq_min_slider_.setRange(freq_absolute_min, freq_initial_max);
    freq_min_slider_.onValueChange = [this, ptr = &freq_min_slider_]() { on_freq_slider_changed(ptr); };
    freq_min_slider_.setValue(freq_initial_min, juce::dontSendNotification);

    addAndMakeVisible(freq_max_label_);
    freq_max_label_.setText("Max Frequency", juce::dontSendNotification);
    addAndMakeVisible(freq_max_slider_);
    freq_max_slider_.setRange(freq_initial_min, freq_absolute_max);
    freq_max_slider_.onValueChange = [this, ptr = &freq_max_slider_]() { on_freq_slider_changed(ptr); };
    freq_max_slider_.setValue(freq_initial_max);

    addAndMakeVisible(freq_distance_label_);
    freq_distance_label_.setText("Frequency Distance (m)", juce::dontSendNotification);
    addAndMakeVisible(freq_distance_slider_);
    freq_distance_slider_.setRange(0.01, 1.0);
    freq_distance_slider_.onValueChange = [this]() { on_freq_range_slider_changed(); };
    freq_distance_slider_.setValue(0.5);

    addAndMakeVisible(freq_scaling_label_);
    freq_scaling_label_.setText("Frequency Scaling Method", juce::dontSendNotification);
    freq_scaling_label_.attachToComponent(&freq_scaling_box_, true);

    addAndMakeVisible(freq_scaling_box_);
    freq_scaling_box_.addItemList({"Linear", "String", "Keyboard", "Theremin"}, 1);
    freq_scaling_box_.onChange = [this]() { on_freq_scaling_changed(); };
    freq_scaling_box_.setSelectedId(3);

    addAndMakeVisible(snapping_label_);
    snapping_label_.setText("Snapping", juce::dontSendNotification);
    snapping_label_.attachToComponent(&snapping_box_, true);

    addAndMakeVisible(snapping_box_);
    snapping_box_.addItemList({"None", "Chromatic", "Pentatonic", "Major"}, 1);
    snapping_box_.onChange = [this]() { on_snapping_changed(); };
    snapping_box_.setSelectedId(1);

    setSize(800, 600);

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
    auto slider_bounds = getLocalBounds().reduced(10);
    auto half_width = slider_bounds.getWidth() / 2;

    live_level_label_.setBounds(slider_bounds.getX(), 10, slider_bounds.getWidth(), 20);
    live_level_slider_.setBounds(slider_bounds.getX(), 40, slider_bounds.getWidth(), 20);
    live_frequency_label_.setBounds(slider_bounds.getX(), 70, slider_bounds.getWidth(), 20);
    live_frequency_slider_.setBounds(slider_bounds.getX(), 100, slider_bounds.getWidth(), 20);

    volume_label_.setBounds(slider_bounds.getX(), 130, slider_bounds.getWidth(), 20);
    volume_slider_.setBounds(slider_bounds.getX(), 160, slider_bounds.getWidth(), 20);

    volume_min_distance_label_.setBounds(slider_bounds.getX(), 190, half_width, 20);
    volume_min_distance_slider_.setBounds(slider_bounds.getX(), 220, half_width, 20);

    volume_max_distance_label_.setBounds(slider_bounds.getX() + half_width, 190, half_width, 20);
    volume_max_distance_slider_.setBounds(slider_bounds.getX() + half_width, 220, half_width, 20);

    freq_min_label_.setBounds(slider_bounds.getX(), 250, half_width, 20);
    freq_min_slider_.setBounds(slider_bounds.getX(), 280, half_width, 20);
    freq_max_label_.setBounds(slider_bounds.getX() + half_width, 250, half_width, 20);
    freq_max_slider_.setBounds(slider_bounds.getX() + half_width, 280, half_width, 20);

    freq_distance_label_.setBounds(slider_bounds.getX(), 310, slider_bounds.getWidth(), 20);
    freq_distance_slider_.setBounds(slider_bounds.getX(), 340, slider_bounds.getWidth(), 20);

    freq_scaling_box_.setBounds(slider_bounds.getX() + half_width, 370, half_width, 20);
    snapping_box_.setBounds(slider_bounds.getX() + half_width, 400, half_width, 20);
}

void MainComponent::prepareToPlay([[maybe_unused]] int samples_per_block, double sample_rate)
{
    audio_processor_.prepare(sample_rate);
}

void MainComponent::releaseResources()
{
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
        live_level_slider_.setValue(message->level, juce::dontSendNotification);
        live_frequency_slider_.setValue(message->frequency, juce::dontSendNotification);
    }
}

void MainComponent::on_freq_slider_changed(juce::Slider* slider_ptr)
{
    if (slider_ptr == &freq_min_slider_) {
        freq_max_slider_.setRange(freq_min_slider_.getValue(), freq_max_slider_.getMaximum());
    } else if (slider_ptr == &freq_max_slider_) {
        freq_min_slider_.setRange(freq_min_slider_.getMinimum(), freq_max_slider_.getValue());
    }
    live_frequency_slider_.setRange(freq_min_slider_.getValue(), freq_max_slider_.getValue());

    repaint();

    frame_processor_.set_frequency_output_bounds(
            Bounds(freq_min_slider_.getValue(), freq_max_slider_.getValue()));
}

void MainComponent::on_freq_range_slider_changed()
{
    frame_processor_.set_frequency_physical_range(freq_distance_slider_.getValue());
}

void MainComponent::on_volume_slider_changed()
{
    live_level_slider_.setRange(0.0, volume_slider_.getValue());
    frame_processor_.set_max_level(volume_slider_.getValue());
    repaint();
}

void MainComponent::on_volume_range_slider_changed(juce::Slider* slider_ptr)
{
    if (slider_ptr == &volume_min_distance_slider_) {
        volume_max_distance_slider_.setRange(
                volume_min_distance_slider_.getValue(),
                volume_max_distance_slider_.getMaximum());
    } else if (slider_ptr == &volume_max_distance_slider_) {
        volume_min_distance_slider_.setRange(
                volume_min_distance_slider_.getMinimum(),
                volume_max_distance_slider_.getValue());
    }

    repaint();

    frame_processor_.set_level_physical_bounds(
            Bounds(volume_min_distance_slider_.getValue(),
                   volume_max_distance_slider_.getValue()));
}

void MainComponent::on_freq_scaling_changed()
{
    ScalingMethod method;
    switch (freq_scaling_box_.getSelectedId()) {
        case (1):
            method = ScalingMethod::Linear;
            break;
        case (2):
            method = ScalingMethod::String;
            break;
        case (3):
            method = ScalingMethod::Keyboard;
            break;
        case (4):
            method = ScalingMethod::Theremin;
            break;
        default:
            // This should never happen
            return;
    }
    frame_processor_.set_frequency_scaling(method);
}

void MainComponent::on_snapping_changed()
{
    SnappingMode mode;
    switch (snapping_box_.getSelectedId()) {
        case (1):
            mode = SnappingMode::None;
            break;
        case (2):
            mode = SnappingMode::Chromatic;
            break;
        case (3):
            mode = SnappingMode::Pentatonic;
            break;
        case (4):
            mode = SnappingMode::Major;
            break;
        default:
            // This should never happen
            return;
    }
    frame_processor_.set_snapping_mode(mode);
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
