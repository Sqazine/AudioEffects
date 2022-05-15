/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


class AudioPlayerPluginAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::ChangeListener
{
public:
    AudioPlayerPluginAudioProcessorEditor (AudioPlayerPluginAudioProcessor&);
    ~AudioPlayerPluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void changeState(TransportState newState);
private:
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;

    TransportState state;

    std::unique_ptr<juce::FileChooser> chooser;

    AudioPlayerPluginAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPlayerPluginAudioProcessorEditor)
};
