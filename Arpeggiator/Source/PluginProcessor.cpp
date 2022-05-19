/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ArpeggiatorAudioProcessor::ArpeggiatorAudioProcessor()
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
    addParameter(speed = new juce::AudioParameterFloat("speed", "Arpeggiator Speed", 0.0, 1.0, 0.5));
}

ArpeggiatorAudioProcessor::~ArpeggiatorAudioProcessor()
{
}

//==============================================================================
const juce::String ArpeggiatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ArpeggiatorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ArpeggiatorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ArpeggiatorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ArpeggiatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ArpeggiatorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ArpeggiatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ArpeggiatorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ArpeggiatorAudioProcessor::getProgramName (int index)
{
    return {};
}

void ArpeggiatorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ArpeggiatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    notes.clear();
    currentNote = 0;
    lastNoteValue = -1;
    time = 0;
    rate = static_cast<float>(sampleRate);
}

void ArpeggiatorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ArpeggiatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ArpeggiatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    jassert(buffer.getNumChannels() == 0);
    auto numSamples = buffer.getNumSamples();

    auto noteDuration = static_cast<int>(std::ceil(rate*0.25f*(0.1f+(1.0f-(*speed)))));

    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
            notes.add(msg.getNoteNumber());
        else if (msg.isNoteOff())
            notes.removeValue(msg.getNoteNumber());
    }

    midiMessages.clear();

    if ((time + numSamples) >= noteDuration)
    {
        auto offset = juce::jmax(0, juce::jmin((int)(noteDuration - time), numSamples - 1));

        if (lastNoteValue > 0)
        {
            midiMessages.addEvent(juce::MidiMessage::noteOff(1,lastNoteValue),offset);
            lastNoteValue = -1;
        }
        
        if (notes.size() > 0)
        {
            currentNote = (currentNote + 1) % notes.size();
            lastNoteValue = notes[currentNote];
            midiMessages.addEvent(juce::MidiMessage::noteOn(1,lastNoteValue,(juce::uint8)127),offset);
        }
    }

    time = (time + numSamples) % noteDuration;
}

//==============================================================================
bool ArpeggiatorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ArpeggiatorAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void ArpeggiatorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ArpeggiatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ArpeggiatorAudioProcessor();
}
