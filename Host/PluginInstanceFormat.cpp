

#include <JuceHeader.h>
#include "PluginInstanceFormat.h"
#include "PluginInstanceProxy.h"
#include "PluginGraph.h"

#include "PluginInstanceIncludedHeader.inl"

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
     #include "PluginInstanceFactoryCreation.inl" 
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
