# lab-audio-effects
A series of audio effect plugins created personally using JUCE,just for fun

## Build
```sh
install CMake(version >= 3.22)
install C++ Compiler(GCC,Clang MSVC,etc)

git submoddule update --init --recursive
mkdir build
cd build
cmake ..
cmake --build .

#then executable and vst3 plugin all listed in (build/Bin/) folder 
```