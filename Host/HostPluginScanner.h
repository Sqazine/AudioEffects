#pragma once
#include <JuceHeader.h>

constexpr const char* g_ScanModeKey = "pluginScanMode";

class Superprocess final : private ChildProcessCoordinator
{
public:
    Superprocess()
    {
        launchWorkerProcess(File::getSpecialLocation(File::currentExecutableFile), processUID, 0, 0);
    }

    enum class State
    {
        timeout,
        gotResult,
        connectionLost,
    };

    struct Response
    {
        State state;
        std::unique_ptr<XmlElement> xml;
    };

    Response getResponse()
    {
        std::unique_lock<std::mutex> lock{mutex};

        if (!condvar.wait_for(lock, std::chrono::milliseconds{50}, [&]
                              { return gotResult || connectionLost; }))
            return {State::timeout, nullptr};

        const auto state = connectionLost ? State::connectionLost : State::gotResult;
        connectionLost = false;
        gotResult = false;

        return {state, std::move(pluginDescription)};
    }

    using ChildProcessCoordinator::sendMessageToWorker;

private:
    void handleMessageFromWorker(const MemoryBlock &mb) override
    {
        const std::lock_guard<std::mutex> lock{mutex};
        pluginDescription = parseXML(mb.toString());
        gotResult = true;
        condvar.notify_one();
    }

    void handleConnectionLost() override
    {
        const std::lock_guard<std::mutex> lock{mutex};
        connectionLost = true;
        condvar.notify_one();
    }

    std::mutex mutex;
    std::condition_variable condvar;

    std::unique_ptr<XmlElement> pluginDescription;
    bool connectionLost = false;
    bool gotResult = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Superprocess)
};

class HostPluginScanner final : public KnownPluginList::CustomScanner,
                                  private ChangeListener
{
public:
    HostPluginScanner()
    {
        if (auto *file = getAppProperties().getUserSettings())
            file->addChangeListener(this);

        handleChange();
    }

    ~HostPluginScanner() override
    {
        if (auto *file = getAppProperties().getUserSettings())
            file->removeChangeListener(this);
    }

    bool findPluginTypesFor(AudioPluginFormat &format,
                            OwnedArray<PluginDescription> &result,
                            const String &fileOrIdentifier) override
    {
        if (scanInProcess)
        {
            superprocess = nullptr;
            format.findAllTypesForFile(result, fileOrIdentifier);
            return true;
        }

        if (addPluginDescriptions(format.getName(), fileOrIdentifier, result))
            return true;

        superprocess = nullptr;
        return false;
    }

    void scanFinished() override
    {
        superprocess = nullptr;
    }

private:
    /*  Scans for a plugin with format 'formatName' and ID 'fileOrIdentifier' using a subprocess,
        and adds discovered plugin descriptions to 'result'.

        Returns true on success.

        Failure indicates that the subprocess is unrecoverable and should be terminated.
    */
    bool addPluginDescriptions(const String &formatName,
                               const String &fileOrIdentifier,
                               OwnedArray<PluginDescription> &result)
    {
        if (superprocess == nullptr)
            superprocess = std::make_unique<Superprocess>();

        MemoryBlock block;
        MemoryOutputStream stream{block, true};
        stream.writeString(formatName);
        stream.writeString(fileOrIdentifier);

        if (!superprocess->sendMessageToWorker(block))
            return false;

        for (;;)
        {
            if (shouldExit())
                return true;

            const auto response = superprocess->getResponse();

            if (response.state == Superprocess::State::timeout)
                continue;

            if (response.xml != nullptr)
            {
                for (const auto *item : response.xml->getChildIterator())
                {
                    auto desc = std::make_unique<PluginDescription>();

                    if (desc->loadFromXml(*item))
                        result.add(std::move(desc));
                }
            }

            return (response.state == Superprocess::State::gotResult);
        }
    }

    void handleChange()
    {
        if (auto *file = getAppProperties().getUserSettings())
            scanInProcess = (file->getIntValue(g_ScanModeKey) == 0);
    }

    void changeListenerCallback(ChangeBroadcaster *) override
    {
        handleChange();
    }

    std::unique_ptr<Superprocess> superprocess;

    std::atomic<bool> scanInProcess{true};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HostPluginScanner)
};
