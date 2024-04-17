#pragma once
#include <memory>
#include <vector>

#include "Node.hpp"

namespace mc
{
class NodeFactory;

class MidiPortWatchdog final
{
public:
    MidiPortWatchdog() = delete;

    static void check_nodes(std::vector<std::shared_ptr<Node>>& nodes, NodeFactory& node_factory);
};

} // namespace mc
