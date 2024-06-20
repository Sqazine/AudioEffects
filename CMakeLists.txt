cmake_minimum_required(VERSION 3.22)

project(lab-audio-effect VERSION 0.0.1)

set_directory_properties(PROPERTIES
    JUCE_COMPANY_NAME       "Sqazine"
    JUCE_COMPANY_EMAIL      "Sqazine@163.com")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

add_subdirectory(JUCE)

set(JUCE_COPY_PLUGIN_AFTER_BUILD ON)
set(JUCE_VST3_COPY_DIR ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

add_subdirectory(PluginTestHost)
add_subdirectory(AudioPlayer)
add_subdirectory(3BandEqualizer)
add_subdirectory(Delay)
add_subdirectory(Distortion)
add_subdirectory(Filter)
add_subdirectory(Flanger)
add_subdirectory(NoiseGate)
add_subdirectory(Oscillator)
add_subdirectory(PingPongDelay)
add_subdirectory(Reverb)
add_subdirectory(SimpleDistortion)
add_subdirectory(SimpleEQ)
