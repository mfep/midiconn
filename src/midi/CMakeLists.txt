add_library(midi 
    InputObserver.cpp
    MidiEngine.cpp
    MidiGraph.cpp
    MidiProbe.cpp
    OutputObserver.cpp
)

target_compile_features(midi PUBLIC cxx_std_20)
target_link_libraries(midi PRIVATE fmt::fmt rtmidi spdlog::spdlog)
