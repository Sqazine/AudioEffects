#pragma once
#include <JuceHeader.h>
#include "PluginInstanceFormat.h"
class PluginInstanceProxy final : public AudioPluginInstance
{
public:
    explicit PluginInstanceProxy (std::unique_ptr<AudioProcessor> innerIn)
        : inner (std::move (innerIn))
    {
        jassert (inner != nullptr);

        for (auto isInput : { true, false })
            matchChannels (isInput);

        setBusesLayout (inner->getBusesLayout());
    }

   
    const String getName() const override                                         { return inner->getName(); }
    StringArray getAlternateDisplayNames() const override                         { return inner->getAlternateDisplayNames(); }
    double getTailLengthSeconds() const override                                  { return inner->getTailLengthSeconds(); }
    bool acceptsMidi() const override                                             { return inner->acceptsMidi(); }
    bool producesMidi() const override                                            { return inner->producesMidi(); }
    AudioProcessorEditor* createEditor() override                                 { return inner->createEditor(); }
    bool hasEditor() const override                                               { return inner->hasEditor(); }
    int getNumPrograms() override                                                 { return inner->getNumPrograms(); }
    int getCurrentProgram() override                                              { return inner->getCurrentProgram(); }
    void setCurrentProgram (int i) override                                       { inner->setCurrentProgram (i); }
    const String getProgramName (int i) override                                  { return inner->getProgramName (i); }
    void changeProgramName (int i, const String& n) override                      { inner->changeProgramName (i, n); }
    void getStateInformation (juce::MemoryBlock& b) override                      { inner->getStateInformation (b); }
    void setStateInformation (const void* d, int s) override                      { inner->setStateInformation (d, s); }
    void getCurrentProgramStateInformation (juce::MemoryBlock& b) override        { inner->getCurrentProgramStateInformation (b); }
    void setCurrentProgramStateInformation (const void* d, int s) override        { inner->setCurrentProgramStateInformation (d, s); }
    void prepareToPlay (double sr, int bs) override                               { inner->setRateAndBufferSizeDetails (sr, bs); inner->prepareToPlay (sr, bs); }
    void releaseResources() override                                              { inner->releaseResources(); }
    void memoryWarningReceived() override                                         { inner->memoryWarningReceived(); }
    void processBlock (AudioBuffer<float>& a, MidiBuffer& m) override             { inner->processBlock (a, m); }
    void processBlock (AudioBuffer<double>& a, MidiBuffer& m) override            { inner->processBlock (a, m); }
    void processBlockBypassed (AudioBuffer<float>& a, MidiBuffer& m) override     { inner->processBlockBypassed (a, m); }
    void processBlockBypassed (AudioBuffer<double>& a, MidiBuffer& m) override    { inner->processBlockBypassed (a, m); }
    bool supportsDoublePrecisionProcessing() const override                       { return inner->supportsDoublePrecisionProcessing(); }
    bool supportsMPE() const override                                             { return inner->supportsMPE(); }
    bool isMidiEffect() const override                                            { return inner->isMidiEffect(); }
    void reset() override                                                         { inner->reset(); }
    void setNonRealtime (bool b) noexcept override                                { inner->setNonRealtime (b); }
    void refreshParameterList() override                                          { inner->refreshParameterList(); }
    void numChannelsChanged() override                                            { inner->numChannelsChanged(); }
    void numBusesChanged() override                                               { inner->numBusesChanged(); }
    void processorLayoutsChanged() override                                       { inner->processorLayoutsChanged(); }
    void setPlayHead (AudioPlayHead* p) override                                  { inner->setPlayHead (p); }
    void updateTrackProperties (const TrackProperties& p) override                { inner->updateTrackProperties (p); }
    bool isBusesLayoutSupported (const BusesLayout& layout) const override        { return inner->checkBusesLayoutSupported (layout); }
    bool applyBusLayouts (const BusesLayout& layouts) override                    { return inner->setBusesLayout (layouts) && AudioPluginInstance::applyBusLayouts (layouts); }

    bool canAddBus (bool) const override                                          { return true; }
    bool canRemoveBus (bool) const override                                       { return true; }

   
    void fillInPluginDescription (PluginDescription& description) const override
    {
        description = getPluginDescription (*inner);
    }

private:
    static PluginDescription getPluginDescription (const AudioProcessor& proc)
    {
        const auto ins                  = proc.getTotalNumInputChannels();
        const auto outs                 = proc.getTotalNumOutputChannels();
        const auto identifier           = proc.getName();
        const auto registerAsGenerator  = ins == 0;
        const auto acceptsMidi          = proc.acceptsMidi();

        PluginDescription descr;

        descr.name              = identifier;
        descr.descriptiveName   = identifier;
        descr.pluginFormatName  = PluginInstanceFormat::getIdentifier();
        descr.category          = (registerAsGenerator ? (acceptsMidi ? "Synth" : "Generator") : "Effect");
        descr.manufacturerName  = "JUCE";
        descr.version           = ProjectInfo::versionString;
        descr.fileOrIdentifier  = identifier;
        descr.isInstrument      = (acceptsMidi && registerAsGenerator);
        descr.numInputChannels  = ins;
        descr.numOutputChannels = outs;

        descr.uniqueId = descr.deprecatedUid = identifier.hashCode();

        return descr;
    }

    void matchChannels (bool isInput)
    {
        const auto inBuses = inner->getBusCount (isInput);

        while (getBusCount (isInput) < inBuses)
            addBus (isInput);

        while (inBuses < getBusCount (isInput))
            removeBus (isInput);
    }

    std::unique_ptr<AudioProcessor> inner;

   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginInstanceProxy)
};