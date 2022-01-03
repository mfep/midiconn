#pragma once
#include <variant>
#include <vector>
#include "MidiEngine.hpp"
#include "Node.hpp"

namespace mc::display
{

class NodeEditor final
{
public:
    void render();

private:
    void renderContextMenu();
    void renderNodes();
    void handleDelete();
    void handleConnect();

    std::vector<midi::InputInfo> m_input_infos;
    std::vector<midi::OutputInfo> m_output_infos;
    std::vector<std::shared_ptr<Node>> m_nodes;
};

}
