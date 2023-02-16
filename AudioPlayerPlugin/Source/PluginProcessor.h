#pragma once

#include <JuceHeader.h>

enum TransportState
{
	Stopped,
	Starting,
	Playing,
    Pausing,
    Paused,
	Stopping,
};

class AudioPlayerPluginAudioProcessor  : public juce::AudioProcessor,public juce::ChangeListener
{
public:
    
    AudioPlayerPluginAudioProcessor();
    ~AudioPlayerPluginAudioProcessor() override;

    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void changeState(TransportState newState);
    void loadFile(juce::File& file);
	TransportState state;
    juce::AudioTransportSource transportSource;
	juce::AudioFormatManager formatManager;
    float volume;
    float gain;
private:
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;


    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPlayerPluginAudioProcessor)
};
