#pragma once
#include <JuceHeader.h>
#include "Common/PluginParameterSlider.h"
#include "Common/PluginParameterComboBox.h"
#include "Common/PluginParameterToggle.h"

class ChorusAudioProcessor : public juce::AudioProcessor
{
public:
    ChorusAudioProcessor();
    ~ChorusAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    juce::AudioProcessorEditor *createEditor() override;
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
    void changeProgramName(int index, const juce::String &newName) override;

    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

private:
    enum WaveformIndex
    {
        SINE = 0,
        TRIANGLE,
        SWATOOTH,
        INVERSE_SWATOOTH
    };
    enum InterpolationIndex
    {
        NEAREST_NEIGHBOUR = 0,
        LINEAR,
        CUBIC
    };
    const juce::StringArray mWaveformItemsUI =
        {
            "Sine",
            "Triangle",
            "Sawtooth(Rising)",
            "Sawtooth(Falling)",
        };

    const juce::StringArray mInterpolationItemsUI =
        {
            "NearestNeighbour",
            "Linear",
            "Cubic",
        };

    float Lfo(float phase, int32_t waveform);
    float mLfoPhase;
    float mInverseSampleRate;

    juce::AudioSampleBuffer mDelayBuffer;
    int32_t mDelayBufferSamples;
    int32_t mDelayBufferChannels;
    int32_t mDelayWritePosition;

    juce::AudioProcessorValueTreeState mApvts;
    PluginParameterSlider mDelay;
    PluginParameterSlider mWidth;
    PluginParameterSlider mDepth;
    PluginParameterComboBox mNumVoices;
    PluginParameterSlider mFrequency;
    PluginParameterComboBox mWaveform;
    PluginParameterComboBox mInterpolation;
    PluginParameterToggle mStereo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusAudioProcessor)
};