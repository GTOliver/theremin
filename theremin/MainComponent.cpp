#include "juce_events/juce_events.h"

#include "MainComponent.h"

MainComponent::MainComponent()
        : tracker_{},
          audio_processor_{},
          messages_{},
          latest_message_{}
{
    tracker_.set_callback([this](TrackingFrame frame) { tracking_callback(frame); });

    double vol_absolute_min = 0.0;
    double vol_absolute_max = 1.0;
    double vol_initial_min = 0.0;
    double vol_initial_max = 0.5;

    double freq_absolute_min = 8.0;
    double freq_absolute_max = 10000.0;
    double freq_initial_min = 440.0;
    double freq_initial_max = 1760.0;

    double adsr_attack = 100.0;
    double adsr_decay = 200.0;
    double adsr_sustain = 0.8;
    double adsr_release = 200.0;

    addAndMakeVisible(live_level_label_);
    live_level_label_.setText("Level", juce::dontSendNotification);
    addAndMakeVisible(live_level_slider_);
    live_level_slider_.setRange(0.0, 0.2);

    addAndMakeVisible(live_frequency_label_);
    live_frequency_label_.setText("Frequency", juce::dontSendNotification);
    addAndMakeVisible(live_frequency_slider_);
    live_frequency_slider_.setRange(freq_initial_min, freq_initial_max);
    live_frequency_slider_.setSkewFactorFromMidPoint(1000.0);

    addAndMakeVisible(volume_min_label_);
    volume_min_label_.setText("Min Volume", juce::dontSendNotification);
    addAndMakeVisible(volume_min_slider_);
    volume_min_slider_.setRange(vol_absolute_min, vol_initial_max);
    volume_min_slider_.onValueChange = [this]() { on_volume_slider_changed(); };
    volume_min_slider_.setValue(vol_initial_min, juce::dontSendNotification);

    addAndMakeVisible(volume_max_label_);
    volume_max_label_.setText("Max Volume", juce::dontSendNotification);
    addAndMakeVisible(volume_max_slider_);
    volume_max_slider_.setRange(vol_initial_min, vol_absolute_max);
    volume_max_slider_.onValueChange = [this]() { on_volume_slider_changed(); };
    volume_max_slider_.setValue(vol_initial_max);

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
    snapping_box_.addItemList({"None", "Chromatic", "Pentatonic", "Major", "Chromatic Envelope",
                               "17-tone Chromatic", "17-tone Major", "17-tone Pentatonic", "11-tone Pentatonic", "14-tone Pentatonic"},
                              1);
    snapping_box_.onChange = [this]() { on_snapping_changed(); };
    snapping_box_.setSelectedId(1);

    addAndMakeVisible(adsr_attack_label_);
    adsr_attack_label_.setText("Attack", juce::dontSendNotification);
    adsr_attack_label_.attachToComponent(&adsr_attack_slider_, true);
    addAndMakeVisible(adsr_attack_slider_);
    adsr_attack_slider_.setRange(0.0, 500.0);
    adsr_attack_slider_.onValueChange = [this]() { on_adsr_changed(); };
    adsr_attack_slider_.setValue(adsr_attack);

    addAndMakeVisible(adsr_decay_label_);
    adsr_decay_label_.setText("Decay", juce::dontSendNotification);
    adsr_decay_label_.attachToComponent(&adsr_decay_slider_, true);
    addAndMakeVisible(adsr_decay_slider_);
    adsr_decay_slider_.setRange(0.0, 500.0);
    adsr_decay_slider_.onValueChange = [this]() { on_adsr_changed(); };
    adsr_decay_slider_.setValue(adsr_decay);

    addAndMakeVisible(adsr_sustain_slider_);
    adsr_sustain_label_.setText("Sustain", juce::dontSendNotification);
    adsr_sustain_label_.attachToComponent(&adsr_sustain_slider_, true);
    addAndMakeVisible(adsr_sustain_slider_);
    adsr_sustain_slider_.setRange(0.0, 1.0);
    adsr_sustain_slider_.onValueChange = [this]() { on_adsr_changed(); };
    adsr_sustain_slider_.setValue(adsr_sustain);

    addAndMakeVisible(adsr_release_label_);
    adsr_release_label_.setText("Release", juce::dontSendNotification);
    adsr_release_label_.attachToComponent(&adsr_release_slider_, true);
    addAndMakeVisible(adsr_release_slider_);
    adsr_release_slider_.setRange(0.0, 1000.0);
    adsr_release_slider_.onValueChange = [this]() { on_adsr_changed(); };
    adsr_release_slider_.setValue(adsr_release);

    addAndMakeVisible(finger_tip_button_);
    finger_tip_button_.onStateChange = [this]() { on_tip_changed(); };

    addAndMakeVisible(square_blend_label_);
    square_blend_label_.setText("Square Blend", juce::dontSendNotification);
    addAndMakeVisible(square_blend_slider_);
    square_blend_slider_.setRange(0.0, 1.0);
    square_blend_slider_.onValueChange = [this]() { on_square_blend_changed(); };
    square_blend_slider_.setValue(0.0, juce::dontSendNotification);

    addAndMakeVisible(square_blend_on_grab_);
    square_blend_on_grab_.onStateChange = [this]() { on_square_blend_on_grab_changed(); };

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
    auto rhs_x = slider_bounds.getX() + half_width;

    live_level_label_.setBounds(slider_bounds.getX(), 10, slider_bounds.getWidth(), 20);
    live_level_slider_.setBounds(slider_bounds.getX(), 40, slider_bounds.getWidth(), 20);
    live_frequency_label_.setBounds(slider_bounds.getX(), 70, slider_bounds.getWidth(), 20);
    live_frequency_slider_.setBounds(slider_bounds.getX(), 100, slider_bounds.getWidth(), 20);

    volume_min_label_.setBounds(slider_bounds.getX(), 130, half_width, 20);
    volume_min_slider_.setBounds(slider_bounds.getX(), 160, half_width, 20);
    volume_max_label_.setBounds(rhs_x, 130, half_width, 20);
    volume_max_slider_.setBounds(rhs_x, 160, half_width, 20);

    volume_min_distance_label_.setBounds(slider_bounds.getX(), 190, half_width, 20);
    volume_min_distance_slider_.setBounds(slider_bounds.getX(), 220, half_width, 20);

    volume_max_distance_label_.setBounds(rhs_x, 190, half_width, 20);
    volume_max_distance_slider_.setBounds(rhs_x, 220, half_width, 20);

    freq_min_label_.setBounds(slider_bounds.getX(), 250, half_width, 20);
    freq_min_slider_.setBounds(slider_bounds.getX(), 280, half_width, 20);
    freq_max_label_.setBounds(rhs_x, 250, half_width, 20);
    freq_max_slider_.setBounds(rhs_x, 280, half_width, 20);

    freq_distance_label_.setBounds(slider_bounds.getX(), 310, slider_bounds.getWidth(), 20);
    freq_distance_slider_.setBounds(slider_bounds.getX(), 340, slider_bounds.getWidth(), 20);

    freq_scaling_box_.setBounds(rhs_x, 370, half_width, 20);
    snapping_box_.setBounds(rhs_x, 400, half_width, 20);

    adsr_attack_slider_.setBounds(rhs_x, 430, half_width, 20);
    adsr_decay_slider_.setBounds(rhs_x, 460, half_width, 20);
    adsr_sustain_slider_.setBounds(rhs_x, 490, half_width, 20);
    adsr_release_slider_.setBounds(rhs_x, 520, half_width, 20);

    finger_tip_button_.setBounds(rhs_x, 550, half_width, 20);

    square_blend_label_.setBounds(slider_bounds.getX(), 460, half_width / 2, 20);
    square_blend_slider_.setBounds(slider_bounds.getX(), 490, half_width / 2, 20);
    square_blend_on_grab_.setBounds(slider_bounds.getX(), 550, half_width, 20);
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
        if (square_blend_on_grab_.getToggleState()) {
            square_blend_slider_.setValue(message->cc);
        }
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
    live_level_slider_.setRange(volume_min_slider_.getMinimum(), volume_max_slider_.getValue());
    frame_processor_.set_level_bounds(Bounds(volume_min_slider_.getValue(), volume_max_slider_.getValue()));
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
        case (5):
            mode = SnappingMode::NoneWithChromaticEnvelope;
            break;
        case (6):
            mode = SnappingMode::Chromatic17;
            break;
        case (7):
            mode = SnappingMode::Major17;
            break;
        case (8):
            mode = SnappingMode::Pentatonic17;
            break;
        case (9):
            mode = SnappingMode::Pentatonic11;
            break;
        case (10):
            mode = SnappingMode::Pentatonic14;
            break;
        default:
            // This should never happen
            return;
    }
    frame_processor_.set_snapping_mode(mode);
}

void MainComponent::on_adsr_changed()
{
    audio_processor_.set_attack(adsr_attack_slider_.getValue());
    audio_processor_.set_decay(adsr_decay_slider_.getValue());
    audio_processor_.set_sustain(adsr_sustain_slider_.getValue());
    audio_processor_.set_release(adsr_release_slider_.getValue());
}

void MainComponent::on_tip_changed()
{
    frame_processor_.set_use_fingers_enabled(finger_tip_button_.getToggleState());
}

void MainComponent::on_square_blend_changed()
{
    if (!square_blend_on_grab_.getToggleState()) {
        audio_processor_.set_square_blend(square_blend_slider_.getValue());
    }
}

void MainComponent::on_square_blend_on_grab_changed()
{
    audio_processor_.set_enable_cc(square_blend_on_grab_.getToggleState());
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
