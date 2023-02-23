#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class AudioPlayerPluginAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::ChangeListener, public juce::Timer
{
public:
	AudioPlayerPluginAudioProcessorEditor(AudioPlayerPluginAudioProcessor&);
	~AudioPlayerPluginAudioProcessorEditor() override;

	void paint(juce::Graphics&) override;
	void resized() override;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

	void timerCallback() override;
private:
	typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
	typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

	const int32_t leftInterval = 10;
	const int32_t rightInterval = 10;
	const int32_t topInterval = 10;
	const int32_t bottomInterval = 10;
	const int32_t elementSize = 30;
	const float sliderWidthPercent = 0.8;

	juce::TextButton openButton;
	juce::TextButton playOrStopButton;
	juce::Slider volumeSlider;
	juce::Slider gainSlider;

	std::unique_ptr<SliderAttachment> gainAttachment;
	std::unique_ptr<SliderAttachment> volumeAttachment;
	std::unique_ptr<ButtonAttachment> playOrStopButtonAttachment;

	juce::Rectangle<int> thumbnailBounds;

	std::unique_ptr<juce::FileChooser> chooser;

	juce::AudioThumbnailCache thumbnailCache;
	juce::AudioThumbnail thumbnail;
	juce::AudioFormatManager formatManager;

	AudioPlayerPluginAudioProcessor& audioProcessor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlayerPluginAudioProcessorEditor)
};
