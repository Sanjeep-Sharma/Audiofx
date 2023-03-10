/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                           juce::Slider::TextEntryBoxPosition::NoTextBox)
    {

    }
};

struct CustomLinearSlider : juce::Slider
{
    CustomLinearSlider() : juce::Slider(juce::Slider::SliderStyle::LinearVertical,
                           juce::Slider::TextEntryBoxPosition::NoTextBox)
    {

    }
};



//==============================================================================
/**
*/
class AudioFXAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    AudioFXAudioProcessorEditor(AudioFXAudioProcessor&);
    ~AudioFXAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioFXAudioProcessor& audioProcessor;

    CustomRotarySlider peakFreqSlider,
        peakGainSlider,
        peakQualitySlider,
        lowCutoffSlider,
        highCutoffSlider,
        gainSlider,
        roomSizeSlider,
        widthSlider;

    CustomLinearSlider drySlider,
        wetSlider;


    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment peakFreqSliderAttachment,
               peakGainSliderAttachment,
               peakQualitySliderAttachment,
               lowCutoffSliderAttachment,
               highCutoffSliderAttachment,
               gainSliderAttachment,
               roomSizeSliderAttachment,
               widthSliderAttachment,
               drySliderAttachment,
               wetSliderAttachment;

    std::vector<juce::Component*> getComps();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioFXAudioProcessorEditor)
};
