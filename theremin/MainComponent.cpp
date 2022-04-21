#include "MainComponent.h"
#include "LeapC.h"

MainComponent::MainComponent() {
    setSize(600, 400);
}

void MainComponent::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setFont(juce::Font(16.0f));
    g.setColour(juce::Colours::white);
    juce::String my_str;
    my_str << LeapGetNow();
    g.drawText(my_str, getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized() {
}
