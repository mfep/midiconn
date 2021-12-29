#pragma once
#include <variant>
#include <vector>
#include "MidiEngine.hpp"

namespace mc::display
{

class NodeEditor final
{
public:
    void render();

private:
    struct Node
    {
        std::variant<midi::InputInfo, midi::OutputInfo> m_info;
        int m_id;

        const std::string& get_name() const;
    };

    void renderContextMenu();
    void renderNodes();
    void handleDelete();

    std::vector<midi::InputInfo> m_input_infos;
    std::vector<midi::OutputInfo> m_output_infos;
    std::vector<Node> m_nodes;
    int m_current_node_id{};
};

}
