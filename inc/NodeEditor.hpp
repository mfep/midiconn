#pragma once
#include <memory>
#include <vector>

#include "nlohmann/json_fwd.hpp"

#include "MidiInfo.hpp"

namespace mc
{
class Node;
class NodeFactory;

namespace display
{

class NodeEditor final
{
public:
    NodeEditor(const NodeFactory& node_factory);

    void              render();
    void              to_json(nlohmann::json& j) const;
    static NodeEditor from_json(const NodeFactory& node_factory, const nlohmann::json& j);

private:
    void renderContextMenu();
    void renderNodes();
    void handleDelete();
    void handleConnect();

    std::vector<midi::InputInfo>       m_input_infos;
    std::vector<midi::OutputInfo>      m_output_infos;
    std::vector<std::shared_ptr<Node>> m_nodes;
    const NodeFactory*                 m_node_factory;
};

} // namespace display
} // namespace mc
