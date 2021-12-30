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

    struct Link
    {
        int m_id;
        int m_start_id;
        int m_end_id;
    };

    void renderContextMenu();
    void renderNodes();
    void handleDelete();
    void handleConnect();

    std::vector<midi::InputInfo> m_input_infos;
    std::vector<midi::OutputInfo> m_output_infos;
    std::vector<Node> m_nodes;
    std::vector<Link> m_links;
    int m_current_node_id{};
    int m_current_link_id{};
};

}
