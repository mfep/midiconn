add_library(midi STATIC
    ChannelMap.cpp
    ChannelMapNode.cpp
    InputNode.cpp
    MidiGraph.cpp
    MidiProbe.cpp
    GraphObserver.cpp
    OutputNode.cpp
)

target_compile_features(midi PUBLIC cxx_std_20)
target_link_libraries(midi PRIVATE fmt::fmt rtmidi spdlog::spdlog)
