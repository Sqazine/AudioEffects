#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


class AudioPlayerPluginAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::ChangeListener,public juce::Timer
{
public:
	AudioPlayerPluginAudioProcessorEditor(AudioPlayerPluginAudioProcessor&);
	~AudioPlayerPluginAudioProcessorEditor() override;

	void paint(juce::Graphics&) override;
	void resized() override;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;
	void changeState(TransportState newState);

	void paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
	void paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);

	void timerCallback() override;
private:
	juce::TextButton openButton;
	juce::TextButton playButton;
	juce::TextButton stopButton;

	TransportState state;

	std::unique_ptr<juce::FileChooser> chooser;

	juce::AudioThumbnailCache thumbnailCache;
	juce::AudioThumbnail thumbnail;
	juce::AudioFormatManager formatManager;

	AudioPlayerPluginAudioProcessor& audioProcessor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlayerPluginAudioProcessorEditor)
};
