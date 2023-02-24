/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioFXAudioProcessor::AudioFXAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
}

AudioFXAudioProcessor::~AudioFXAudioProcessor()
{
}

//==============================================================================
const juce::String AudioFXAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioFXAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioFXAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioFXAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioFXAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioFXAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int AudioFXAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioFXAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String AudioFXAudioProcessor::getProgramName(int index)
{
    return {};
}

void AudioFXAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void AudioFXAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec specs;
    specs.sampleRate = sampleRate;
    specs.maximumBlockSize = samplesPerBlock;
    specs.numChannels = 1;

    leftChain.prepare(specs);
    rightChain.prepare(specs);

    auto chainsettings = getChainSettings(apvts);

    updateDistortion(chainsettings);
    updateLowCut(chainsettings);
    updatePeak(chainsettings);
    updateHighCut(chainsettings);
    updateReverb(chainsettings);
}

void AudioFXAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AudioFXAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void AudioFXAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    //This is where we change the sound

    auto chainsettings = getChainSettings(apvts);

    updateDistortion(chainsettings);
    updateLowCut(chainsettings);
    updatePeak(chainsettings);
    updateHighCut(chainsettings);
    updateReverb(chainsettings);


    //This rest of the code processes the audio according to the dsp and its parameters
    juce::dsp::AudioBlock<float> block(buffer);

    auto leftblock = block.getSingleChannelBlock(0);
    auto rightblock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftcontext(leftblock);
    juce::dsp::ProcessContextReplacing<float> rightcontext(rightblock);

    leftChain.process(leftcontext);
    rightChain.process(rightcontext);
}

//==============================================================================
bool AudioFXAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioFXAudioProcessor::createEditor()
{
    return new AudioFXAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void AudioFXAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void AudioFXAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);

        auto chainsettings = getChainSettings(apvts);

        updateDistortion(chainsettings);
        updateLowCut(chainsettings);
        updatePeak(chainsettings);
        updateHighCut(chainsettings);
        updateReverb(chainsettings);
        
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioFXAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout

AudioFXAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    //This is for Distortion
    layout.add(std::make_unique<juce::AudioParameterFloat>("Gain", "Gain", juce::NormalisableRange<float>(0.0f, 30.f, 0.1f, 1.f), 0.0f));

    //This is for Filter
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowCutoff", "LowCutoff", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 20.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("HighCutoff", "HighCutoff", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 20000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakFreq", "PeakFreq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 2000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakQuality", "PeakQuality", juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakGain", "PeakGain", juce::NormalisableRange<float>(-20.f, 20.f, 0.5f, 1.f), 0.0f));

    //This is for Reverb
    layout.add(std::make_unique<juce::AudioParameterFloat>("RoomSize", "RoomSize", juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.0f), 0.3f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Width", "Width", juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.0f), 0.3f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Dry", "Dry", juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.0f), 1.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Wet", "Wet", juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.0f), 1.f));

    return layout;
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings tempsetting;
    tempsetting.Gain = apvts.getRawParameterValue("Gain")->load();

    tempsetting.LowCutoff = apvts.getRawParameterValue("LowCutoff")->load();
    tempsetting.PeakFreq = apvts.getRawParameterValue("PeakFreq")->load();
    tempsetting.HighCutoff = apvts.getRawParameterValue("HighCutoff")->load();
    tempsetting.PeakGain = apvts.getRawParameterValue("PeakGain")->load();
    tempsetting.PeakQuality = apvts.getRawParameterValue("PeakQuality")->load();

    tempsetting.RoomSize = apvts.getRawParameterValue("RoomSize")->load();
    tempsetting.Width = apvts.getRawParameterValue("Width")->load();
    tempsetting.Wet = apvts.getRawParameterValue("Wet")->load();
    tempsetting.Dry = apvts.getRawParameterValue("Dry")->load();


    return tempsetting;
}

void AudioFXAudioProcessor::updateDistortion(const ChainSettings& settings)
{
    auto gainindecibals = settings.Gain;
    auto gain = juce::Decibels::decibelsToGain(gainindecibals);
    auto lambda = [gain](float in) ->float
    {
        return std::tanh(gain * in);
    };

    leftChain.get<chainPosition::WaveShape>().functionToUse = lambda;
    rightChain.get<chainPosition::WaveShape>().functionToUse = lambda;
}

void AudioFXAudioProcessor::updateLowCut(const ChainSettings& settings)
{
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), settings.LowCutoff, 1);

    *leftChain.get<chainPosition::LowCut>().coefficients = *coeffs;
    *rightChain.get<chainPosition::LowCut>().coefficients = *coeffs;

}
void AudioFXAudioProcessor::updateHighCut(const ChainSettings& settings)
{
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), settings.HighCutoff, 1);

    *leftChain.get<chainPosition::HighCut>().coefficients = *coeffs;
    *rightChain.get<chainPosition::HighCut>().coefficients = *coeffs;

}
void AudioFXAudioProcessor::updatePeak(const ChainSettings& settings)
{
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), settings.PeakFreq, settings.PeakQuality, juce::Decibels::decibelsToGain(settings.PeakGain));

    *leftChain.get<chainPosition::Peak>().coefficients = *coeffs;
    *rightChain.get<chainPosition::Peak>().coefficients = *coeffs;

}
void AudioFXAudioProcessor::updateReverb(const ChainSettings& settings)
{
    juce::Reverb::Parameters param;
    param.damping = settings.RoomSize;
    param.roomSize = settings.RoomSize;
    param.freezeMode = false;
    param.width = settings.Width;
    param.wetLevel = settings.Wet;
    param.dryLevel = settings.Dry;

    leftChain.get<chainPosition::Reverberation>().setParameters(param);
    rightChain.get<chainPosition::Reverberation>().setParameters(param);

}