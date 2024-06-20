/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#include "JuceHeader.h"
#include "InternalPluginFormat.h"
#include "PluginGraph.h"


InternalPluginFormat::InternalPluginFactory::InternalPluginFactory (const std::initializer_list<Constructor>& constructorsIn)
    : constructors (constructorsIn),
      descriptions ([&]
      {
          std::vector<PluginDescription> result;

          for (const auto& constructor : constructors)
              result.push_back (constructor()->getPluginDescription());

          return result;
      }())
{}

std::unique_ptr<AudioPluginInstance> InternalPluginFormat::InternalPluginFactory::createInstance (const String& name) const
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

InternalPluginFormat::InternalPluginFormat()
    : factory {
        [] { return std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor> (AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode); },
        [] { return std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor> (AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode); },
        [] { return std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor> (AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode); },
        [] { return std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor> (AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode); },

        /*   [] { return std::make_unique<InternalPlugin> (std::make_unique<SineWaveSynth>()); },
           [] { return std::make_unique<InternalPlugin> (std::make_unique<ReverbPlugin>()); },

         [] { return std::make_unique<InternalPlugin>(std::make_unique<AUv3SynthProcessor>()); },
         [] { return std::make_unique<InternalPlugin>(std::make_unique<Arpeggiator>()); },
         [] { return std::make_unique<InternalPlugin>(std::make_unique<DspModulePluginDemoAudioProcessor>()); },
         [] { return std::make_unique<InternalPlugin>(std::make_unique<GainProcessor>()); },
         [] { return std::make_unique<InternalPlugin>(std::make_unique<JuceDemoPluginAudioProcessor>()); },
         [] { return std::make_unique<InternalPlugin>(std::make_unique<MidiLoggerPluginDemoProcessor>()); },
         [] { return std::make_unique<InternalPlugin>(std::make_unique<MultiOutSynth>()); },
         [] { return std::make_unique<InternalPlugin>(std::make_unique<NoiseGate>()); },
         [] { return std::make_unique<InternalPlugin>(std::make_unique<SamplerAudioProcessor>()); },
         [] { return std::make_unique<InternalPlugin>(std::make_unique<SurroundProcessor>()); }*/
    }
{
}

std::unique_ptr<AudioPluginInstance> InternalPluginFormat::createInstance (const String& name)
{
    return factory.createInstance (name);
}

void InternalPluginFormat::createPluginInstance (const PluginDescription& desc,
                                                 double /*initialSampleRate*/, int /*initialBufferSize*/,
                                                 PluginCreationCallback callback)
{
    if (auto p = createInstance (desc.name))
        callback (std::move (p), {});
    else
        callback (nullptr, NEEDS_TRANS ("Invalid internal plugin name"));
}

bool InternalPluginFormat::requiresUnblockedMessageThreadDuringCreation (const PluginDescription&) const
{
    return false;
}

const std::vector<PluginDescription>& InternalPluginFormat::getAllTypes() const
{
    return factory.getDescriptions();
}
