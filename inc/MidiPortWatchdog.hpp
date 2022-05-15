#pragma once
#include <memory>
#include <vector>

#include "Node.hpp"

namespace mc
{
namespace midi
{
class Engine;
}

class MidiPortWatchdog final
{
public:
    MidiPortWatchdog() = delete;

    static void check_nodes(std::vector<std::shared_ptr<Node>>& nodes, midi::Engine& midi_engine);
};

} // namespace mc
