#pragma once
#include <memory>
#include <vector>

#include "nlohmann/json_fwd.hpp"

#include "MidiInfo.hpp"

namespace mc
{
class Node;
namespace midi
{
class Engine;
}
} // namespace mc
namespace mc::display
{

class NodeEditor final
{
public:
    NodeEditor(midi::Engine& midi_engine);

    void              render();
    void              to_json(nlohmann::json& j) const;
    static NodeEditor from_json(midi::Engine& midi_engine, const nlohmann::json& j);

private:
    void renderContextMenu();
    void renderNodes();
    void handleDelete();
    void handleConnect();

    std::vector<midi::InputInfo>       m_input_infos;
    std::vector<midi::OutputInfo>      m_output_infos;
    std::vector<std::shared_ptr<Node>> m_nodes;
    midi::Engine*                      m_midi_engine;
};

} // namespace mc::display
