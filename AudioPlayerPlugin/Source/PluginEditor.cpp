#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioPlayerPluginAudioProcessorEditor::AudioPlayerPluginAudioProcessorEditor(AudioPlayerPluginAudioProcessor& p)
	: AudioProcessorEditor(&p),
	audioProcessor(p),
	state(p.state),
	thumbnailCache(5),
	thumbnail(512, p.formatManager, thumbnailCache)
{
	startTimer(40);

	thumbnail.addChangeListener(this);

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
			thumbnail.setSource(new juce::FileInputSource(file));
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

	juce::Rectangle<int> thumbnailBounds(10, 100, getWidth() - 20, getHeight() - 120);

	if (thumbnail.getNumChannels() == 0)
		paintIfNoFileLoaded(g, thumbnailBounds);
	else
		paintIfFileLoaded(g, thumbnailBounds);
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

	if (source == &thumbnail)
		repaint();
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
			openButton.setEnabled(false);
			break;
		case Playing:
			playButton.setButtonText("Pause");
			stopButton.setButtonText("Stop");
			stopButton.setEnabled(true);
			break;
		case Pausing:
			openButton.setEnabled(true);
			break;
		case Paused:
			openButton.setEnabled(true);
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

void AudioPlayerPluginAudioProcessorEditor::paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
	g.setColour(juce::Colour::fromRGB(50, 77, 107));
	g.fillRect(thumbnailBounds);
	g.setColour(juce::Colour::fromRGB(207, 229, 252));
	g.drawFittedText("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
}

void AudioPlayerPluginAudioProcessorEditor::paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
	g.setColour(juce::Colour::fromRGB(50, 77, 107));
	g.fillRect(thumbnailBounds);
	g.setColour(juce::Colour::fromRGB(207, 229, 252));

	auto audioLength = thumbnail.getTotalLength();

	thumbnail.drawChannels(g, thumbnailBounds, 0.0, audioLength, 1.0f);

	g.setColour(juce::Colours::green);

	auto audioPosition = (float)audioProcessor.transportSource.getCurrentPosition();
	auto drawPosition = (audioPosition / audioLength) * (float)thumbnailBounds.getWidth() + (float)thumbnailBounds.getX();

	g.drawLine(drawPosition, (float)thumbnailBounds.getY(), drawPosition, (float)thumbnailBounds.getBottom(), 2.0f);
}

void AudioPlayerPluginAudioProcessorEditor::timerCallback()
{
	repaint();
}
