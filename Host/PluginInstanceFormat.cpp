

#include "JuceHeader.h"
#include "PluginInstanceFormat.h"
#include "PluginInstanceProxy.h"
#include "PluginGraph.h"

PluginInstanceFormat::PluginFactory::PluginFactory(const std::initializer_list<Constructor>& constructorsIn)
    : constructors (constructorsIn),
      descriptions ([&]
      {
          std::vector<PluginDescription> result;

          for (const auto& constructor : constructors)
              result.push_back (constructor()->getPluginDescription());

          return result;
      }())
{}

std::unique_ptr<AudioPluginInstance> PluginInstanceFormat::PluginFactory::createInstance (const String& name) const
{
    const auto begin = descriptions.begin();
    const auto it = std::find_if (begin,
                                  descriptions.end(),
                                  [&] (const PluginDescription& desc) { return name.equalsIgnoreCase (desc.name); });

    if (it == descriptions.end())
        return nullptr;

    const auto index = (size_t) std::distance (begin, it);
    return constructors[index]();
}

PluginInstanceFormat::PluginInstanceFormat()
    : factory {
        [] { return std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor> (AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode); },
        [] { return std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor> (AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode); },
        [] { return std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor> (AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode); },
        [] { return std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor> (AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode); },

           [] { return std::make_unique<PluginInstanceProxy> (std::make_unique<ReverbPlugin>()); },
        /*   [] { return std::make_unique<PluginInstanceProxy> (std::make_unique<SineWaveSynth>()); },
           [] { return std::make_unique<PluginInstanceProxy> (std::make_unique<ReverbPlugin>()); },

         [] { return std::make_unique<PluginInstanceProxy>(std::make_unique<AUv3SynthProcessor>()); },
         [] { return std::make_unique<PluginInstanceProxy>(std::make_unique<Arpeggiator>()); },
         [] { return std::make_unique<PluginInstanceProxy>(std::make_unique<DspModulePluginDemoAudioProcessor>()); },
         [] { return std::make_unique<PluginInstanceProxy>(std::make_unique<GainProcessor>()); },
         [] { return std::make_unique<PluginInstanceProxy>(std::make_unique<JuceDemoPluginAudioProcessor>()); },
         [] { return std::make_unique<PluginInstanceProxy>(std::make_unique<MidiLoggerPluginDemoProcessor>()); },
         [] { return std::make_unique<PluginInstanceProxy>(std::make_unique<MultiOutSynth>()); },
         [] { return std::make_unique<PluginInstanceProxy>(std::make_unique<NoiseGate>()); },
         [] { return std::make_unique<PluginInstanceProxy>(std::make_unique<SamplerAudioProcessor>()); },
         [] { return std::make_unique<PluginInstanceProxy>(std::make_unique<SurroundProcessor>()); }*/
    }
{
}

std::unique_ptr<AudioPluginInstance> PluginInstanceFormat::createInstance (const String& name)
{
    return factory.createInstance (name);
}

void PluginInstanceFormat::createPluginInstance (const PluginDescription& desc,
                                                 double /*initialSampleRate*/, int /*initialBufferSize*/,
                                                 PluginCreationCallback callback)
{
    if (auto p = createInstance (desc.name))
        callback (std::move (p), {});
    else
        callback (nullptr, NEEDS_TRANS ("Invalid internal plugin name"));
}

bool PluginInstanceFormat::requiresUnblockedMessageThreadDuringCreation (const PluginDescription&) const
{
    return false;
}

const std::vector<PluginDescription>& PluginInstanceFormat::getAllTypes() const
{
    return factory.getDescriptions();
}
