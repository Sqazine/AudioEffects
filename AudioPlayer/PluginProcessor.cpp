#include "PluginProcessor.h"
#include "PluginEditor.h"


AudioPlayerAudioProcessor::AudioPlayerAudioProcessor()
	: AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
	parameters(*this, nullptr, juce::Identifier("AudioPlayerParams"),
		{
			std::make_unique<juce::AudioParameterFloat>("Gain","Gain",juce::NormalisableRange<float>(0.0f,1.0f,0.01f),0.5f,"",juce::AudioProcessorParameter::genericParameter,
			[](float value,int)
				{
			return static_cast<juce::String>((int)(value*127.0f));
				},
			[](const juce::String& string)
				{
			return string.getFloatValue()/127.0f;
				}),
			std::make_unique<juce::AudioParameterFloat>("Volume","Volume",juce::NormalisableRange<float>(0.0f,1.0f,0.01f),0.5f,"",juce::AudioProcessorParameter::genericParameter,
			[](float value,int)
				{
			return static_cast<juce::String>((int)(value * 127.0f));
				},
			[](const juce::String& string)
				{
			return string.getFloatValue() / 127.0f;
				})
		})
{
	formatManager.registerBasicFormats();
}

AudioPlayerAudioProcessor::~AudioPlayerAudioProcessor()
{
	transportSource.setSource(nullptr);
}


const juce::String AudioPlayerAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool AudioPlayerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool AudioPlayerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool AudioPlayerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double AudioPlayerAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int AudioPlayerAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int AudioPlayerAudioProcessor::getCurrentProgram()
{
	return 0;
}

void AudioPlayerAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String AudioPlayerAudioProcessor::getProgramName(int index)
{
	return {};
}

void AudioPlayerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}


void AudioPlayerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	transportSource.prepareToPlay(samplesPerBlock, sampleRate);
}

void AudioPlayerAudioProcessor::releaseResources()
{
	transportSource.releaseResources();
	transportSource.setSource(nullptr);
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AudioPlayerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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

void AudioPlayerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	/*auto gainLength = parameters.getParameterRange("Gain").getRange().getLength();
	auto volumeLength = parameters.getParameterRange("Volume").getRange().getLength();*/
	auto gain = parameters.getRawParameterValue("Gain")->load();
	auto volume = parameters.getRawParameterValue("Volume")->load();

	juce::AudioSourceChannelInfo info(&buffer, 0, buffer.getNumSamples());
	transportSource.getNextAudioBlock(info);
	transportSource.setGain(gain);

	auto numSamples = buffer.getNumSamples();
	for (int32_t i = 0; i < buffer.getNumChannels(); ++i)
	{
		const float* inData = buffer.getReadPointer(i);
		float* outData = buffer.getWritePointer(i);
		for (int32_t j = 0; j < numSamples; ++j)
		{
			outData[j] = inData[j] * volume;
		}
	}
}


bool AudioPlayerAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPlayerAudioProcessor::createEditor()
{
	return new AudioPlayerAudioProcessorEditor(*this);
}


void AudioPlayerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	auto state = parameters.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void AudioPlayerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(parameters.state.getType()))
			parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void AudioPlayerAudioProcessor::loadFile(juce::File& file)
{
	auto reader = formatManager.createReaderFor(file);
	if (reader)
	{
		auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
		transportSource.stop();
		transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
		readerSource.reset(newSource.release());
	}
}

// This creates new instances of the plugin..
#ifdef EXPORT_CREATE_FILTER_FUNCTION
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new AudioPlayerAudioProcessor();
}
#endif