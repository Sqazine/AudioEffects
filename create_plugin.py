import sys
import os

template_name=""
isSuccessfully=True

def cmake_file_content():
    return '''set(PLUGIN_NAME '''+template_name+''')
juce_add_plugin(${PLUGIN_NAME}
    # VERSION ...                               # Set this if the plugin version is different to the project version
    ICON_BIG  "${CMAKE_SOURCE_DIR}/JUCE/extras/AudioPluginHost/Source/JUCEAppIcon.png"
    # ICON_SMALL ...
    # COMPANY_NAME ...                          # Specify the name of the plugin's author
    # IS_SYNTH TRUE/FALSE                       # Is this a synth or an effect?
    # NEEDS_MIDI_INPUT TRUE/FALSE               # Does the plugin need midi input?
    # NEEDS_MIDI_OUTPUT TRUE/FALSE              # Does the plugin need midi output?
    # IS_MIDI_EFFECT TRUE/FALSE                 # Is this plugin a MIDI effect?
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    PLUGIN_MANUFACTURER_CODE Sqaz               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Sqaz                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS VST3
    VST3_AUTO_MANIFEST FALSE
    PRODUCT_NAME "${PLUGIN_NAME}") 

juce_generate_juce_header(${PLUGIN_NAME})


file(GLOB_RECURSE SRC "*.h" "*.cpp" "*.inl")
file(GLOB_RECURSE COMMON_SRC "${CMAKE_SOURCE_DIR}/Common/*.h" "${CMAKE_SOURCE_DIR}/Common/*.cpp" "${CMAKE_SOURCE_DIR}/Common/*.inl")
source_group("${PLUGIN_NAME}" FILES ${SRC})
source_group("Common" FILES ${COMMON_SRC})

target_sources(${PLUGIN_NAME} PRIVATE ${SRC} ${COMMON_SRC})

target_compile_definitions(${PLUGIN_NAME} PUBLIC
                            JUCE_WEB_BROWSER=0  
                            JUCE_USE_CURL=0     
                            JUCE_VST3_CAN_REPLACE_VST2=0)

target_compile_definitions(${PLUGIN_NAME}_VST3 PRIVATE EXPORT_CREATE_FILTER_FUNCTION)
target_sources(${PLUGIN_NAME}_VST3 PRIVATE ${SRC} ${COMMON_SRC})
target_include_directories(${PLUGIN_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(${PLUGIN_NAME}
    PRIVATE
        juce::juce_analytics
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

set_property(TARGET ${PLUGIN_NAME}_rc_lib PROPERTY FOLDER ${PLUGIN_NAME})'''


def plugin_processor_hpp_file_content():
    return '''#pragma once
#include <JuceHeader.h>

class '''+template_name+'''AudioProcessor  : public juce::AudioProcessor
{
public:
    
    '''+template_name+'''AudioProcessor();
    ~'''+template_name+'''AudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR ('''+template_name+'''AudioProcessor)
};'''

def plugin_processor_cpp_file_content():
    return '''
#include "PluginProcessor.h"
#include "PluginEditor.h"

'''+template_name+'''AudioProcessor::'''+template_name+'''AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

'''+template_name+'''AudioProcessor::~'''+template_name+'''AudioProcessor()
{
}

const juce::String '''+template_name+'''AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool '''+template_name+'''AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool '''+template_name+'''AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool '''+template_name+'''AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double '''+template_name+'''AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int '''+template_name+'''AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int '''+template_name+'''AudioProcessor::getCurrentProgram()
{
    return 0;
}

void '''+template_name+'''AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String '''+template_name+'''AudioProcessor::getProgramName (int index)
{
    return {};
}

void '''+template_name+'''AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void '''+template_name+'''AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void '''+template_name+'''AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool '''+template_name+'''AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void '''+template_name+'''AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

bool '''+template_name+'''AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* '''+template_name+'''AudioProcessor::createEditor()
{
    return new '''+template_name+'''AudioProcessorEditor (*this);
}

void '''+template_name+'''AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void '''+template_name+'''AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


// This creates new instances of the plugin..
#ifdef EXPORT_CREATE_FILTER_FUNCTION
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new '''+template_name+'''AudioProcessor();
}
#endif
'''

def plugin_editor_hpp_file_content():
    return '''#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class '''+template_name+'''AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    '''+template_name+'''AudioProcessorEditor ('''+template_name+'''AudioProcessor&);
    ~'''+template_name+'''AudioProcessorEditor() override;

    
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    '''+template_name+'''AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR ('''+template_name+'''AudioProcessorEditor)
};
'''

def plugin_editor_cpp_file_content():
    return '''#include "PluginProcessor.h"
#include "PluginEditor.h"

'''+template_name+'''AudioProcessorEditor::'''+template_name+'''AudioProcessorEditor ('''+template_name+'''AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

'''+template_name+'''AudioProcessorEditor::~'''+template_name+'''AudioProcessorEditor()
{
}

void '''+template_name+'''AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void '''+template_name+'''AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
'''

def create_folder():
    global isSuccessfully
    exists = os.path.exists(template_name)
    if not exists:
        os.makedirs(template_name)
        print("\033[0;32m", "folder create successfully:",template_name,"\033[0m")
    else:
        print("\033[0;31m", "error:already exists a folder named:",template_name,"\033[0m")
        isSuccessfully = False


def create_file(path,content_callback):
    global isSuccessfully
    exists = os.path.exists(path)
    if not exists:
        file=open(path,"w")
        file.write(content_callback())
        file.close()
        print("\033[0;32m", "file create successfully:",path,"\033[0m")
    else:
        print("\033[0;31m", "error:already exists a file named:",path,"\033[0m")
        isSuccessfully = False
        
def create_cmake_file():
    path=template_name+"/CMakeLists.txt"
    create_file(path,cmake_file_content)


def create_plugin_precessor_hpp_file():
    path=template_name+"/PluginProcessor.h"
    create_file(path,plugin_processor_hpp_file_content)

def create_plugin_precessor_cpp_file():
    path=template_name+"/PluginProcessor.cpp"
    create_file(path,plugin_processor_cpp_file_content)

def create_plugin_editor_hpp_file():
    path=template_name+"/PluginEditor.h"
    create_file(path,plugin_editor_hpp_file_content)

def create_plugin_editor_cpp_file():
    path=template_name+"/PluginEditor.cpp"
    create_file(path,plugin_editor_cpp_file_content)

def append_major_cmake_file():
    global isSuccessfully
    if isSuccessfully == True:
        file=open("CMakeLists.txt","a")
        file.write("\nadd_subdirectory("+template_name+")")
        file.close()

def append_plugin_instance_factory_file():
    global isSuccessfully
    if isSuccessfully == True:
        file=open("Host/PluginInstanceFactoryCreation.inl","a")
        file.write("\n[] { return std::make_unique<PluginInstanceProxy> (std::make_unique<"+template_name+"AudioProcessor>()); },")
        file.close()

def append_plugin_instance_header_file():
    global isSuccessfully
    if isSuccessfully == True:
        file=open("Host/PluginInstanceIncludedHeader.inl","a")
        file.write("\n#include \""+template_name+"/PluginProcessor.h\"")
        file.close()


def append_cmake_link_library_file():
    global isSuccessfully
    if isSuccessfully == True:
        file=open("Host/CMakeLinkLibraries.cmake","r+",encoding="utf-8")

        lines = file.readlines()
        lines.pop()
        lines.append("\t"+template_name+"\n")
        lines.append(")")

        file.seek(0)
        file.truncate()
        
        file.writelines(lines)
        file.flush()
        file.close()


def print_usage():
    print("python3 create_template [template_name],such as:python3 create_template Delay")
    exit(1)

if __name__ == "__main__":
    if len(sys.argv)<2:
        print_usage()
    
    template_name=sys.argv[1]

    create_folder()
    create_cmake_file()
    create_plugin_precessor_hpp_file()
    create_plugin_precessor_cpp_file()
    create_plugin_editor_hpp_file()
    create_plugin_editor_cpp_file()
    append_major_cmake_file()

    append_plugin_instance_header_file()
    append_plugin_instance_factory_file()    
    append_cmake_link_library_file()

