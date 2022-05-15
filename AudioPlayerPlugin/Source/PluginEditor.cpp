#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioPlayerPluginAudioProcessorEditor::AudioPlayerPluginAudioProcessorEditor(AudioPlayerPluginAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p), state(p.state)
{
	audioProcessor.transportSource.addChangeListener(this);
	setSize(300, 200);
	addAndMakeVisible(&openButton);
	openButton.setButtonText("open...");
	openButton.onClick = [this] {
		chooser = std::make_unique<juce::FileChooser>("select a .wav/.mp3/.aiff/.ogg/.flac/.wma file to play...", juce::File(), "*.wav;*.mp3;*.aiff;*.ogg;*.flac;*.wma");
		auto chooseFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
		chooser->launchAsync(chooseFlags, [this](const juce::FileChooser& fc) {
			auto file = fc.getResult();
			if (file != juce::File{})
			{
				audioProcessor.loadFile(file);
				playButton.setEnabled(true);
			}
			});
	};

	addAndMakeVisible(&playButton);
	playButton.setButtonText("play");
	playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
	playButton.setEnabled(false);
	playButton.onClick = [this]
	{
		if ((state == Stopped) || (state == Paused))
		{
			changeState(Starting);
			audioProcessor.changeState(TransportState::Starting);
		}
		else if (state == Playing)
		{
			changeState(Pausing);
			audioProcessor.changeState(TransportState::Pausing);
		}
	};

	addAndMakeVisible(&stopButton);
	stopButton.setButtonText("stop");
	stopButton.onClick = [this]
	{
		if (state == Paused)
		{
			changeState(Stopped);
			audioProcessor.changeState(TransportState::Stopped);
		}
		else
		{
			changeState(Stopping);
			audioProcessor.changeState(TransportState::Stopping);
		}
	};
	stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
	stopButton.setEnabled(false);
}

AudioPlayerPluginAudioProcessorEditor::~AudioPlayerPluginAudioProcessorEditor()
{
}

void AudioPlayerPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	g.setColour(juce::Colours::white);
}

void AudioPlayerPluginAudioProcessorEditor::resized()
{
	openButton.setBounds(10, 10, getWidth() - 20, 20);
	playButton.setBounds(10, 40, getWidth() - 20, 20);
	stopButton.setBounds(10, 70, getWidth() - 20, 20);
}

void AudioPlayerPluginAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{

	if (source == &audioProcessor.transportSource)
	{
		if (audioProcessor.transportSource.isPlaying())
			changeState(Playing);
		else if ((state == Stopping) || (state == Playing))
			changeState(Stopped);
		else if (Pausing == state)
			changeState(Paused);
	}
}

void AudioPlayerPluginAudioProcessorEditor::changeState(TransportState newState)
{
	if (state != newState)
	{
		state = newState;

		switch (state)
		{
		case Stopped:
			playButton.setButtonText("Play");
			stopButton.setButtonText("Stop");
			stopButton.setEnabled(false);
			break;
		case Starting:
			break;
		case Playing:
			playButton.setButtonText("Pause");
			stopButton.setButtonText("Stop");
			stopButton.setEnabled(true);
			break;
		case Pausing:
			break;
		case Paused:
			playButton.setButtonText("Resume");
			stopButton.setButtonText("Return to Zero");
			break;
		case Stopping:
			break;
		default:
			break;
		}
	}
}
