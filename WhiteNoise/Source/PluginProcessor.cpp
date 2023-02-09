/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

//==============================================================================
WhiteNoiseAudioProcessor::WhiteNoiseAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	),
#endif
	mGain(new juce::AudioParameterFloat("gain","gain",0.0f,1.0f,0.5f)),
	mInvertPhase(new juce::AudioParameterBool("invertPhase","invertPhase",false))
{
	addParameter(mGain);
	addParameter(mInvertPhase);
}

WhiteNoiseAudioProcessor::~WhiteNoiseAudioProcessor()
{
}

//==============================================================================
const juce::String WhiteNoiseAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool WhiteNoiseAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool WhiteNoiseAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool WhiteNoiseAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double WhiteNoiseAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int WhiteNoiseAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int WhiteNoiseAudioProcessor::getCurrentProgram()
{
	return 0;
}

void WhiteNoiseAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String WhiteNoiseAudioProcessor::getProgramName(int index)
{
	return {};
}

void WhiteNoiseAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void WhiteNoiseAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	auto phase = *mInvertPhase ? -1.0f : 1.0f;
	mPreviousGain = *mGain;
}

void WhiteNoiseAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
	
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WhiteNoiseAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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

	return true;
#endif
}
#endif

void WhiteNoiseAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	auto numSamples = buffer.getNumSamples();
	for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel)
	{
		auto* channelData = buffer.getWritePointer(channel);

		for (auto sample = 0; sample < numSamples; ++sample)
			channelData[sample] = mRandom.nextFloat()*0.25f-0.125f;
	}

	auto phase = *mInvertPhase ? -1.0f : 1.0f;
	auto curGain = (*mGain) * phase;

	if (curGain == mPreviousGain)
		buffer.applyGain(curGain);
	else
	{
		buffer.applyGainRamp(0, numSamples, mPreviousGain,curGain);
		mPreviousGain = curGain;
	}
}

//==============================================================================
bool WhiteNoiseAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WhiteNoiseAudioProcessor::createEditor()
{
	return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void WhiteNoiseAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void WhiteNoiseAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new WhiteNoiseAudioProcessor();
}
