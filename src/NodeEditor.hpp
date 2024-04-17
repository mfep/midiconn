#pragma once
#include <memory>
#include <vector>

#include "nlohmann/json_fwd.hpp"

#include "midi/MidiInfo.hpp"

namespace mc
{
class Node;
class NodeFactory;
class PortNameDisplay;
class ThemeControl;

class NodeEditor final
{
public:
    NodeEditor(NodeFactory&           node_factory,
               const PortNameDisplay& port_name_display,
               const ThemeControl&    theme_control,
               bool                   create_nodes = false);

    void              render();
    void              to_json(nlohmann::json& j) const;
    static NodeEditor from_json(NodeFactory&           node_factory,
                                const PortNameDisplay& port_name_display,
                                const ThemeControl&    theme_control,
                                const nlohmann::json&  j);

private:
    std::shared_ptr<Node> renderContextMenu(bool show_outputting_nodes = true,
                                            bool show_inputting_nodes  = true);
    void                  renderNodes();
    void                  renderHelpText();
    void                  handleDelete();
    void                  handleConnect();
    void                  handleLinkDropped();
    void                  instantiate_available_inputs_and_outputs();

    std::vector<midi::InputInfo>       m_input_infos;
    std::vector<midi::OutputInfo>      m_output_infos;
    std::vector<std::shared_ptr<Node>> m_nodes;
    NodeFactory*                       m_node_factory;
    int                                m_dropped_link_id{-1};
    const PortNameDisplay*             m_port_name_display;
    const ThemeControl*                m_theme_control;
};

} // namespace mc
