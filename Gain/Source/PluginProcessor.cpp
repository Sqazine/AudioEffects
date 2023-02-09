/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

//==============================================================================
GainAudioProcessor::GainAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(gain = new juce::AudioParameterFloat("gain", "Gain", 0.0f, 1.0f, 0.5f));
    addParameter(invertPhase = new juce::AudioParameterBool("invertPhase", "Invert Phase", false));
}

GainAudioProcessor::~GainAudioProcessor()
{
}

//==============================================================================
const juce::String GainAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GainAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GainAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GainAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GainAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GainAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GainAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GainAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GainAudioProcessor::getProgramName (int index)
{
    return {};
}

void GainAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GainAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    auto phase = *invertPhase ? -1.0f : 1.0f;
    previousGain = *gain;
}

void GainAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GainAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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

void GainAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto phase = *invertPhase ? -1.0f : 1.0f;
    auto currentGain = gain->get()*phase;
    if (currentGain == previousGain)
        buffer.applyGain(currentGain);
    else
    {
        buffer.applyGainRamp(0, buffer.getNumSamples(), previousGain, currentGain);
        previousGain = currentGain;
    }
}

//==============================================================================
bool GainAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GainAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void GainAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
	auto xml = std::make_unique<juce::XmlElement>("Gain");
	xml->setAttribute("gain", (double)*gain);
    xml->setAttribute("invertPhase", *invertPhase);
    copyXmlToBinary(*xml, destData);
}

void GainAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto xmlState = getXmlFromBinary(data, sizeInBytes);
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName("Gain"))
        {
            *gain = (float)xmlState->getDoubleAttribute("gain", 1.0);
            *invertPhase = xmlState->getBoolAttribute("invertPhase", false);
        }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainAudioProcessor();
}
