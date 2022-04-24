#include "NodeSerializer.hpp"

#include <optional>

#include "imnodes.h"
#include "nlohmann/json.hpp"

#include "DisconnectedMidiInNode.hpp"
#include "MidiChannelNode.hpp"
#include "MidiEngine.hpp"
#include "MidiInfo.hpp"
#include "MidiInNode.hpp"
#include "MidiOutNode.hpp"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ImVec2, x, y);

namespace mc
{
namespace midi
{

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InputInfo, m_id, m_name);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(OutputInfo, m_id, m_name);

}

using nlohmann::json;

namespace
{

template<typename Info>
std::optional<Info> get_valid_info(const std::string& name, const std::vector<Info>& infos)
{
    auto found_it = std::find_if(infos.begin(), infos.end(), [&](const auto& info) { return info.m_name == name; });
    if (found_it != infos.end())
    {
        return *found_it;
    }
    return std::nullopt;
}

}

NodeSerializer::NodeSerializer(midi::Engine& midi_engine) :
    m_midi_engine(&midi_engine)
{
}

void NodeSerializer::serialize_node(json& j, const Node& node) const
{
    node.accept_serializer(j, *this);
    j["id"] = node.id();

    std::vector<int> output_connected_node_ids;
    for (auto[link_id, other_node] : node.m_output_connections)
    {
        output_connected_node_ids.push_back(other_node.lock()->id());
    }
    j["output_connection_ids"] = output_connected_node_ids;
    j["position"] = ImNodes::GetNodeGridSpacePos(node.id());
}

void NodeSerializer::serialize_node(json& j, const MidiInNode& node) const
{
    j = json{ { "type", "midi_in" }, { "input_name", node.m_input_info.m_name } };
}

void NodeSerializer::serialize_node(json& j, const DisconnectedMidiInNode& node) const
{
    j = json{ { "type", "midi_in" }, { "input_name", node.m_input_name } };
}

void NodeSerializer::serialize_node(json& j, const MidiOutNode& node) const
{
    j = json{ { "type", "midi_out" }, { "output_name", node.m_output_info.m_name } };
}

void NodeSerializer::serialize_node(json& j, const MidiChannelNode& node) const
{
    j = json{ { "type", "midi_channel" }, { "channels", node.m_channels } };
}

std::shared_ptr<Node> NodeSerializer::deserialize_node(const json& j) const
{
    const auto node_type = j["type"].get<std::string>();
    std::shared_ptr<Node> node;
    if (node_type == "midi_in")
    {
        const auto input_name = j["input_name"].get<std::string>();
        const auto input_info_opt = get_valid_info(input_name, midi::Probe::get_inputs());
        if (input_info_opt.has_value())
        {
            node = std::make_shared<MidiInNode>(input_info_opt.value(), *m_midi_engine);
        }
        else
        {
            node = std::make_shared<DisconnectedMidiInNode>(input_name);
        }
    }
    else if (node_type == "midi_out")
    {
        const auto output_name = j["output_name"].get<std::string>();
        const auto output_info_opt = get_valid_info(output_name, midi::Probe::get_outputs());
        if (output_info_opt.has_value())
        {
            node = std::make_shared<MidiOutNode>(output_info_opt.value(), *m_midi_engine);
        }
    }
    else if (node_type == "midi_channel")
    {
        auto channel_node = std::make_shared<MidiChannelNode>();
        j["channels"].get_to(channel_node->m_channels);
        node = channel_node;
    }
    else
    {
        throw std::logic_error("Unexpected node type");
    }
    const auto current_id = node->m_id = j["id"];
    Node::sm_next_id = std::max(current_id + 1, Node::sm_next_id); // Debt, writing to global state
    ImNodes::SetNodeGridSpacePos(node->id(), j["position"]);
    return node;
}

}