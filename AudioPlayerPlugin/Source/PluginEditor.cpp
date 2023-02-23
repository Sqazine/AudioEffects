#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioPlayerPluginAudioProcessorEditor::AudioPlayerPluginAudioProcessorEditor(AudioPlayerPluginAudioProcessor& p)
	: AudioProcessorEditor(&p),
	audioProcessor(p),
	thumbnailCache(5),
	thumbnail(512, p.formatManager, thumbnailCache)
{
	setSize(300, 300);

	startTimer(40);

	thumbnail.addChangeListener(this);

	audioProcessor.transportSource.addChangeListener(this);

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
			playOrStopButton.setEnabled(true);
			playOrStopButton.setButtonText("play");
			playOrStopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
			thumbnail.setSource(new juce::FileInputSource(file));
		}
			});
	};

	addAndMakeVisible(&playOrStopButton);
	playOrStopButton.setButtonText("play");
	playOrStopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
	playOrStopButton.setEnabled(false);
	playOrStopButton.onClick = [this]
	{
		if (audioProcessor.transportSource.isPlaying())
		{
			playOrStopButton.setButtonText("play");
			playOrStopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
			audioProcessor.transportSource.stop();
		}
		else
		{
			playOrStopButton.setButtonText("stop");
			playOrStopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
			audioProcessor.transportSource.start();
		}
	};


	addAndMakeVisible(&gainSlider);
	gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);	
	gainAttachment.reset(new SliderAttachment(audioProcessor.parameters, "Gain", gainSlider));

	addAndMakeVisible(&volumeSlider);
	volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
	volumeAttachment.reset(new SliderAttachment(audioProcessor.parameters, "Volume", volumeSlider));
}

AudioPlayerPluginAudioProcessorEditor::~AudioPlayerPluginAudioProcessorEditor()
{
}

void AudioPlayerPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	g.setColour(juce::Colours::white);

	g.drawFittedText("Volume", leftInterval + volumeSlider.getWidth(), volumeSlider.getY(), getWidth() - (leftInterval + rightInterval) * (1.0 - sliderWidthPercent), elementSize * 0.67, juce::Justification::left, 1);
	g.drawFittedText("Gain", leftInterval + gainSlider.getWidth(), gainSlider.getY(), getWidth() - (leftInterval + rightInterval) * (1.0 - sliderWidthPercent), elementSize * 0.67, juce::Justification::left, 1);

	if (thumbnail.getNumChannels() == 0)
	{
		g.setColour(juce::Colour::fromRGB(50, 77, 107));
		g.fillRect(thumbnailBounds);
		g.setColour(juce::Colour::fromRGB(207, 229, 252));
		g.drawFittedText("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
	}
	else
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
}

void AudioPlayerPluginAudioProcessorEditor::resized()
{
	auto elementCount = 0;
	thumbnailBounds.setBounds(leftInterval,
		topInterval + elementSize * 0,
		getWidth() - (leftInterval + rightInterval),
		elementSize * 4);

	openButton.setBounds(leftInterval,
		topInterval + thumbnailBounds.getX() + thumbnailBounds.getHeight() + elementSize * (elementCount++),
		getWidth() - (leftInterval + rightInterval),
		elementSize * 0.67);

	playOrStopButton.setBounds(leftInterval,
		topInterval + thumbnailBounds.getX() + thumbnailBounds.getHeight() + elementSize * (elementCount++),
		getWidth() - (leftInterval + rightInterval),
		elementSize * 0.67);

	gainSlider.setBounds(leftInterval,
		topInterval + thumbnailBounds.getX() + thumbnailBounds.getHeight() + elementSize * (elementCount++),
		(getWidth() - (leftInterval + rightInterval)) * sliderWidthPercent,
		elementSize * 0.67);

	volumeSlider.setBounds(leftInterval,
		topInterval + thumbnailBounds.getX() + thumbnailBounds.getHeight() + elementSize * (elementCount++),
		(getWidth() - (leftInterval + rightInterval)) * sliderWidthPercent,
		elementSize * 0.67);


	setSize(300, topInterval + elementSize * elementCount + thumbnailBounds.getHeight() + bottomInterval);
}

void AudioPlayerPluginAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	repaint();
}

void AudioPlayerPluginAudioProcessorEditor::timerCallback()
{
	repaint();
}