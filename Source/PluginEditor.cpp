/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioFXAudioProcessorEditor::AudioFXAudioProcessorEditor (AudioFXAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor(p),
    peakFreqSliderAttachment(audioProcessor.apvts, "PeakFreq", peakFreqSlider),
    peakGainSliderAttachment(audioProcessor.apvts, "PeakGain", peakGainSlider),
    peakQualitySliderAttachment(audioProcessor.apvts, "PeakQuality", peakQualitySlider),
    lowCutoffSliderAttachment(audioProcessor.apvts, "LowCutoff", lowCutoffSlider),
    highCutoffSliderAttachment(audioProcessor.apvts, "HighCutoff", highCutoffSlider),
    gainSliderAttachment(audioProcessor.apvts, "Gain", gainSlider),
    roomSizeSliderAttachment(audioProcessor.apvts, "RoomSize", roomSizeSlider),
    widthSliderAttachment(audioProcessor.apvts, "Width", widthSlider),
    drySliderAttachment(audioProcessor.apvts, "Dry", drySlider),
    wetSliderAttachment(audioProcessor.apvts, "Wet", wetSlider) 
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for (auto* comp : getComps())
    {
        addAndMakeVisible(comp);
    }

    setSize(600, 400);
}

AudioFXAudioProcessorEditor::~AudioFXAudioProcessorEditor()
{

}

//==============================================================================
void AudioFXAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Audio FX!", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioFXAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();
    auto VerticalSliderArea = bounds.removeFromRight(bounds.getWidth() * 0.25);
    auto GainSliderArea = bounds.removeFromLeft(bounds.getWidth() * 0.25);
    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);

    lowCutoffSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
    gainSlider.setBounds(GainSliderArea.removeFromTop(GainSliderArea.getHeight() * 1));

    highCutoffSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    roomSizeSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    widthSlider.setBounds(highCutArea);


    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.4));
    peakQualitySlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    peakGainSlider.setBounds(bounds);

    wetSlider.setBounds(VerticalSliderArea.removeFromRight(VerticalSliderArea.getWidth() * 0.5));
    drySlider.setBounds(VerticalSliderArea);


}

std::vector<juce::Component*> AudioFXAudioProcessorEditor::getComps()
{
    return
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &lowCutoffSlider,
        &highCutoffSlider,
        &gainSlider,
        &roomSizeSlider,
        &widthSlider,
        &drySlider,
        &wetSlider
    };
}
