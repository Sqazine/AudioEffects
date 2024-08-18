#pragma once

#include <JuceHeader.h>
#define _USE_MATH_DEFINES
#include <cmath>

#include "Common/PluginParameterSlider.h"
#include "Common/PluginParameterToggle.h"
#include "Common/PluginParameterCombox.h"

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
