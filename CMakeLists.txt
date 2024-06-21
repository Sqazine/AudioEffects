cmake_minimum_required(VERSION 3.22)

project(lab-audio-effect VERSION 0.0.1)

set_directory_properties(PROPERTIES
    JUCE_COMPANY_NAME       "Sqazine"
    JUCE_COMPANY_EMAIL      "Sqazine@163.com")

add_subdirectory(JUCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Bin/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Bin/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Bin/Release)

add_subdirectory(PluginHost)
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
