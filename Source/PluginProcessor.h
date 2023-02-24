/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/

struct ChainSettings
{
    float Gain, HighCutoff, LowCutoff, PeakFreq, PeakQuality, PeakGain, RoomSize, Width, Dry, Wet;
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);


class AudioFXAudioProcessor : public juce::AudioProcessor
                               #if JucePlugin_Enable_ARA
                                , public juce::AudioProcessorARAExtension
                               #endif
{
public:
    //==============================================================================
    AudioFXAudioProcessor();
    ~AudioFXAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    /// createParameterLayout() is the fucntion that returns parameterlayout object for this Project
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };
    //apvts is the object for valuetreestate
private:

    using Filter = juce::dsp::IIR::Filter<float>;
    using Reverb = juce::dsp::Reverb;
    using Waveshaper = juce::dsp::WaveShaper<float, std::function<float(float)>>;

    using MonoChain = juce::dsp::ProcessorChain < Waveshaper, Filter, Filter, Filter, Reverb >;

    MonoChain leftChain, rightChain;

    enum chainPosition
    {
        WaveShape, LowCut, Peak, HighCut, Reverberation
    };

    //Function Declarations
    void updateDistortion(const ChainSettings& settings);
    void updateLowCut(const ChainSettings& settings);
    void updatePeak(const ChainSettings& settings);
    void updateHighCut(const ChainSettings& settings);
    void updateReverb(const ChainSettings& settings);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioFXAudioProcessor)
};
