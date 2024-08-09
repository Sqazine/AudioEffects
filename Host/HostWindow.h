#pragma once
#include "PluginGraph.h"
#include "GraphEditorPanel.h"

namespace CommandIDs
{
    static const int open                   = 0x30000;
    static const int save                   = 0x30001;
    static const int saveAs                 = 0x30002;
    static const int newFile                = 0x30003;
    static const int showPluginListEditor   = 0x30100;
    static const int showAudioSettings      = 0x30200;
}


inline ApplicationProperties& getAppProperties() { return *getApp().appProperties; }
inline ApplicationCommandManager& getCommandManager() { return getApp().commandManager; }

inline bool isOnTouchDevice()
{
    static bool isTouch = Desktop::getInstance().getMainMouseSource().isTouch();
    return isTouch;
}


constexpr const char* processUID = "lab-audio-effect-host";

class HostWindow final : public DocumentWindow,
                             public MenuBarModel,
                             public ApplicationCommandTarget,
                             public ChangeListener,
                             public FileDragAndDropTarget
{
public:
   
    HostWindow();
    ~HostWindow() override;

   
    void closeButtonPressed() override;
    void changeListenerCallback (ChangeBroadcaster*) override;

    bool isInterestedInFileDrag (const StringArray& files) override;
    void fileDragEnter (const StringArray& files, int, int) override;
    void fileDragMove (const StringArray& files, int, int) override;
    void fileDragExit (const StringArray& files) override;
    void filesDropped (const StringArray& files, int, int) override;

    void menuBarActivated (bool isActive) override;

    StringArray getMenuBarNames() override;
    PopupMenu getMenuForIndex (int topLevelMenuIndex, const String& menuName) override;
    void menuItemSelected (int menuItemID, int topLevelMenuIndex) override;
    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands (Array<CommandID>&) override;
    void getCommandInfo (CommandID, ApplicationCommandInfo&) override;
    bool perform (const InvocationInfo&) override;

    void tryToQuitApplication();

    void createPlugin (const PluginDescription&, Point<int> pos);

    void addPluginsToMenu (PopupMenu&);
    std::optional<PluginDescription> getChosenType (int menuID) const;

    std::unique_ptr<GraphDocumentComponent> graphHolder;

private:
   
    void showAudioSettings();
    
    friend class HostPluginListWindow;
   
    AudioDeviceManager deviceManager;
    AudioPluginFormatManager formatManager;

    std::vector<PluginDescription> pluginTypes;
    KnownPluginList knownPluginList;
    KnownPluginList::SortMethod pluginSortMethod;
    Array<PluginDescription> pluginDescriptionArray;

    std::unique_ptr<HostPluginListWindow> pluginListWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HostWindow)
};
