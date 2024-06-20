#pragma once

#include <JuceHeader.h>
#define _USE_MATH_DEFINES
#include <cmath>




class PluginParameter
	:public juce::LinearSmoothedValue<float>,
	public juce::AudioProcessorValueTreeState::Listener
{
public:
	PluginParameter(juce::AudioProcessorValueTreeState& apvts, const std::function<float(float)> callback = nullptr)
		:apvts(apvts), callback(callback)
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

	juce::AudioProcessorValueTreeState& apvts;

	std::function<float(float)> callback;
};

class PluginParameterSlider :public PluginParameter
{
public:

	PluginParameterSlider(juce::AudioProcessorValueTreeState& apvts,
		const juce::String& paramName,
		const juce::String& labelText,
		float minValue,
		float maxValue,
		float defaultValue,
		std::function<float(float)> callback = nullptr,
		bool logarithmic = false)
		: PluginParameter(apvts, callback)
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

		apvts.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>
			(juce::ParameterID(paramID),
				paramName,
				range,
				defaultValue,
				labelText,
				juce::AudioProcessorParameter::genericParameter,
				[](float value, int) { return juce::String(value, 2); },
				[](const juce::String& text) { return text.getFloatValue(); }
				)
		);

		apvts.addParameterListener(paramID, this);
		updateValue(defaultValue);
	}

	const juce::String& paramName;
	const juce::String& labelText;
	const float minValue;
	const float maxValue;
	const float defaultValue;
};

class PluginParameterToggle :public PluginParameter
{
public:
	PluginParameterToggle(juce::AudioProcessorValueTreeState& apvts,
		const juce::String& paramName,
		const juce::String& label,
		const bool defaultState = false,
		const std::function<float(float)> callback = nullptr)
		:PluginParameter(apvts, callback),
		paramName(paramName),
		defaultState(defaultState)
	{
		auto paramID = paramName.removeCharacters(" ").toLowerCase();
		const juce::StringArray toggleStates = { "False","True" };

		apvts.createAndAddParameter(std::make_unique<juce::AudioParameterBool>
			(juce::ParameterID(paramID),
				paramName,
				defaultState,
				label,
				[toggleStates](bool value, int) { return toggleStates[value]; },
				[toggleStates](const juce::String& text) { return toggleStates.indexOf(text); }
				)
		);

		apvts.addParameterListener(paramID, this);
		updateValue(defaultState);
	}

	const juce::String& paramName;
	const bool defaultState;
};

class PluginParameterCombox :public PluginParameter
{
public:
	PluginParameterCombox(juce::AudioProcessorValueTreeState& apvts,
		const juce::String& paramName,
		const juce::String& label,
		const juce::StringArray& items,
		const int defaultChoice = 0,
		const std::function<float(const float)> callback = nullptr)
		:PluginParameter(apvts, callback),
		paramName(paramName),
		items(items),
		defaultChoice(defaultChoice)
	{
		auto paramID = paramName.removeCharacters(" ").toLowerCase();

		apvts.createAndAddParameter(std::make_unique<juce::AudioParameterChoice>
			(juce::ParameterID(paramID),
				paramName,
				items,
				defaultChoice,
				label,
				[items](int value, int) { return items[value]; },
				[items](const juce::String& text) { return items.indexOf(text); }
				)
		);

		apvts.addParameterListener(paramID, this);
		updateValue(defaultChoice);
	}

	const juce::String& paramName;
	const juce::StringArray items;
	const int defaultChoice;
};

const juce::StringArray gWaveformItemsUI =
{
	"Sine",
	"Triangle",
	"Sawtooth",
	"InverseSawtooth"
};

enum WaveformIndex
{
	SINE=0,
	TRIANGLE,
	SWATOOTH,
	INVERSE_SWATOOTH
};

const juce::StringArray gInterpolationItemsUI = 
{
	"NearestNeighbour",
	"Linear",
	"Cubic"
};

enum InterpolationIndex 
{
	NEAREST_NEIGHBOUR=0,
	LINEAR,
	CUBIC
};

constexpr float TWO_PI = 2.0f * 3.1415926535f;

class FlangerAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
	, public juce::AudioProcessorARAExtension
#endif
{
public:
	
	FlangerAudioProcessor();
	~FlangerAudioProcessor() override;

	
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	juce::AudioSampleBuffer mDelayBuffer;
	int32_t mDelayBufferSamples;
	int32_t mDelayBufferChannels;
	int32_t mDelayWritePosition;

	float mLfoPhase;
	float mInverseSampleRate;

	float Lfo(float phase, int32_t waveform);

	juce::AudioProcessorValueTreeState apvts;
	PluginParameterSlider mDelay;
	PluginParameterSlider mWidth;
	PluginParameterSlider mDepth;
	PluginParameterSlider mFeedback;
	PluginParameterToggle mInverted;
	PluginParameterSlider mFrequency;
	PluginParameterCombox mWaveForm;
	PluginParameterCombox mInterpolation;
	PluginParameterToggle mStereo;
private:
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlangerAudioProcessor)
};
