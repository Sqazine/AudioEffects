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

class PluginParametersManager
{
public:
	PluginParametersManager(juce::AudioProcessor& p)
		: apvts(p, nullptr)
	{
	}

	juce::AudioProcessorValueTreeState apvts;
};

class PluginParameter
	:public juce::LinearSmoothedValue<float>,
	public juce::AudioProcessorValueTreeState::Listener
{
public:
	PluginParameter(PluginParametersManager& parametersManager, const std::function<float(float)> callback = nullptr)
		:parametersManager(parametersManager), callback(callback)
	{

	}

	void updateValue(float value)
	{
		if (callback != nullptr)
			setCurrentAndTargetValue(callback(value));
		else
			setCurrentAndTargetValue(value);
	}

	void parameterChanged(const juce::String& parameterID, float newValue) override
	{
		updateValue(newValue);
	}

	PluginParametersManager& parametersManager;
	std::function<float(float)> callback;
};

class PluginParameterSlider :public PluginParameter
{
public:

	PluginParameterSlider(PluginParametersManager& parametersManager,
		const juce::String& paramName,
		const juce::String& labelText,
		float minValue,
		float maxValue,
		float defaultValue,
		std::function<float(float)> callback = nullptr,
		bool logarithmic = false)
		: PluginParameter(parametersManager, callback)
		, paramName(paramName)
		, labelText(labelText)
		, minValue(minValue)
		, maxValue(maxValue)
		, defaultValue(defaultValue)

	{
		auto paramID = paramName.removeCharacters(" ").toLowerCase();

		juce::NormalisableRange<float> range(minValue, maxValue);
		if (logarithmic)
			range.setSkewForCentre(sqrt(minValue * maxValue));

		parametersManager.apvts.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>
			(juce::ParameterID(paramID),
				paramName,
				range,
				defaultValue,
				labelText,
				juce::AudioProcessorParameter::genericParameter,
				[](float value,int) { return juce::String(value, 2); },
				[](const juce::String& text) { return text.getFloatValue(); }
				)
		);

		parametersManager.apvts.addParameterListener(paramID, this);
		updateValue(defaultValue);
	}

	const juce::String& paramName;
	const juce::String& labelText;
	const float minValue;
	const float maxValue;
	const float defaultValue;
};

class DelayAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
	, public juce::AudioProcessorARAExtension
#endif
{
public:
	//==============================================================================
	DelayAudioProcessor();
	~DelayAudioProcessor() override;

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

private:
	juce::AudioSampleBuffer mDelayBuffer;
	int32_t mDelayBufferSamples;
	int32_t mDelayBufferChannels;
	int32_t mDelayWritePositions;

	PluginParametersManager mDelayParameters;
	PluginParameterSlider mDelayParamDelayTime;
	PluginParameterSlider mDelayParamFeedback;
	PluginParameterSlider mDelayParamMix;

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessor)
};
