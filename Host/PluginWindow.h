#pragma once

#include "IOConfigWindow.h"

class PluginGraph;

/**
    A desktop window containing a plugin's GUI.
*/
class PluginWindow final : public DocumentWindow
{
public:
    enum class Type
    {
        normal = 0,
        generic,
        programs,
        audioIO,
        numTypes
    };

    PluginWindow (AudioProcessorGraph::Node* n, Type t, OwnedArray<PluginWindow>& windowList)
        : DocumentWindow (n->getProcessor()->getName(),
                          LookAndFeel::getDefaultLookAndFeel().findColour (ResizableWindow::backgroundColourId),
                          DocumentWindow::minimiseButton | DocumentWindow::closeButton),
          activeWindowList (windowList),
          node (n), type (t)
    {
        setSize (400, 300);

        if (auto* ui = createProcessorEditor (*node->getProcessor(), type))
        {
            setContentOwned (ui, true);
            setResizable (ui->isResizable(), false);
        }

        setConstrainer (&constrainer);

       #if JUCE_IOS || JUCE_ANDROID
        const auto screenBounds = Desktop::getInstance().getDisplays().getTotalBounds (true).toFloat();
        const auto scaleFactor = jmin ((screenBounds.getWidth()  - 50.0f) / (float) getWidth(),
                                       (screenBounds.getHeight() - 50.0f) / (float) getHeight());

        if (scaleFactor < 1.0f)
        {
            setSize ((int) (scaleFactor * (float) getWidth()),
                     (int) (scaleFactor * (float) getHeight()));
        }

        setTopLeftPosition (20, 20);
       #else
        setTopLeftPosition (node->properties.getWithDefault (getLastXProp (type), Random::getSystemRandom().nextInt (500)),
                            node->properties.getWithDefault (getLastYProp (type), Random::getSystemRandom().nextInt (500)));
       #endif

        node->properties.set (getOpenProp (type), true);

        setVisible (true);
    }

    ~PluginWindow() override
    {
        clearContentComponent();
    }

    void moved() override
    {
        node->properties.set (getLastXProp (type), getX());
        node->properties.set (getLastYProp (type), getY());
    }

    void closeButtonPressed() override
    {
        node->properties.set (getOpenProp (type), false);
        activeWindowList.removeObject (this);
    }

    static String getLastXProp (Type type)    { return "uiLastX_" + getTypeName (type); }
    static String getLastYProp (Type type)    { return "uiLastY_" + getTypeName (type); }
    static String getOpenProp  (Type type)    { return "uiopen_"  + getTypeName (type); }

    OwnedArray<PluginWindow>& activeWindowList;
    const AudioProcessorGraph::Node::Ptr node;
    const Type type;

    BorderSize<int> getBorderThickness() override
    {
       #if JUCE_IOS || JUCE_ANDROID
        const int border = 10;
        return { border, border, border, border };
       #else
        return DocumentWindow::getBorderThickness();
       #endif
    }

private:
    class DecoratorConstrainer final : public BorderedComponentBoundsConstrainer
    {
    public:
        explicit DecoratorConstrainer (DocumentWindow& windowIn)
            : window (windowIn) {}

        ComponentBoundsConstrainer* getWrappedConstrainer() const override
        {
            auto* editor = dynamic_cast<AudioProcessorEditor*> (window.getContentComponent());
            return editor != nullptr ? editor->getConstrainer() : nullptr;
        }

        BorderSize<int> getAdditionalBorder() const override
        {
            const auto nativeFrame = [&]() -> BorderSize<int>
            {
                if (auto* peer = window.getPeer())
                    if (const auto frameSize = peer->getFrameSizeIfPresent())
                        return *frameSize;

                return {};
            }();

            return nativeFrame.addedTo (window.getContentComponentBorder());
        }

    private:
        DocumentWindow& window;
    };

    DecoratorConstrainer constrainer { *this };

    float getDesktopScaleFactor() const override     { return 1.0f; }

    static AudioProcessorEditor* createProcessorEditor (AudioProcessor& processor,
                                                        PluginWindow::Type type)
    {
        if (type == PluginWindow::Type::normal)
        {
            if (processor.hasEditor())
                if (auto* ui = processor.createEditorIfNeeded())
                    return ui;

            type = PluginWindow::Type::generic;
        }

        if (type == PluginWindow::Type::generic)
        {
            auto* result = new GenericAudioProcessorEditor (processor);
            result->setResizeLimits (200, 300, 1'000, 10'000);
            return result;
        }

        if (type == PluginWindow::Type::programs)
            return new ProgramAudioProcessorEditor (processor);

        if (type == PluginWindow::Type::audioIO)
            return new IOConfigWindow (processor);

        jassertfalse;
        return {};
    }

    static String getTypeName (Type type)
    {
        switch (type)
        {
            case Type::normal:     return "Normal";
            case Type::generic:    return "Generic";
            case Type::programs:   return "Programs";
            case Type::audioIO:    return "IO";
            case Type::numTypes:
            default:               return {};
        }
    }

   
    struct ProgramAudioProcessorEditor final : public AudioProcessorEditor
    {
        explicit ProgramAudioProcessorEditor (AudioProcessor& p)
            : AudioProcessorEditor (p)
        {
            setOpaque (true);

            addAndMakeVisible (listBox);
            listBox.updateContent();

            const auto rowHeight = listBox.getRowHeight();

            setSize (400, jlimit (rowHeight, 400, p.getNumPrograms() * rowHeight));
        }

        void paint (Graphics& g) override
        {
            g.fillAll (Colours::grey);
        }

        void resized() override
        {
            listBox.setBounds (getLocalBounds());
        }

    private:
        class Model : public ListBoxModel
        {
        public:
            Model (Component& o, AudioProcessor& p)
                : owner (o), proc (p) {}

            int getNumRows() override
            {
                return proc.getNumPrograms();
            }

            void paintListBoxItem (int rowNumber,
                                   Graphics& g,
                                   int width,
                                   int height,
                                   bool rowIsSelected) override
            {
                const auto textColour = owner.findColour (ListBox::textColourId);

                if (rowIsSelected)
                {
                    const auto defaultColour = owner.findColour (ListBox::backgroundColourId);
                    const auto c = rowIsSelected ? defaultColour.interpolatedWith (textColour, 0.5f)
                                                 : defaultColour;

                    g.fillAll (c);
                }

                g.setColour (textColour);
                g.drawText (proc.getProgramName (rowNumber),
                            Rectangle<int> { width, height }.reduced (2),
                            Justification::left,
                            true);
            }

            void selectedRowsChanged (int row) override
            {
                if (0 <= row)
                    proc.setCurrentProgram (row);
            }

        private:
            Component& owner;
            AudioProcessor& proc;
        };

        Model model { *this, *getAudioProcessor() };
        ListBox listBox { "Programs", &model };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgramAudioProcessorEditor)
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};
