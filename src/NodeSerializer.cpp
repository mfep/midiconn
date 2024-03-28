#include "NodeSerializer.hpp"

#include <optional>

#include "imnodes.h"
#include "nlohmann/json.hpp"

#include "DisconnectedMidiInNode.hpp"
#include "DisconnectedMidiOutNode.hpp"
#include "MidiChannelNode.hpp"
#include "MidiInNode.hpp"
#include "MidiOutNode.hpp"
#include "NodeFactory.hpp"
#include "midi/MidiInfo.hpp"
#include "midi/MidiProbe.hpp"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ImVec2, x, y);

namespace mc
{

using nlohmann::json;

NodeSerializer::NodeSerializer(const NodeFactory& node_factory) : m_node_factory(&node_factory)
{
}

void NodeSerializer::serialize_node(json& j, const Node& node) const
{
    node.accept_serializer(j, *this);
    j["id"] = node.id();

    std::vector<int> output_connected_node_ids;
    for (auto [link_id, other_node] : node.m_output_connections)
    {
        output_connected_node_ids.push_back(other_node.lock()->id());
    }
    j["output_connection_ids"] = output_connected_node_ids;
    j["position"]              = ImNodes::GetNodeGridSpacePos(node.id());
}

void NodeSerializer::serialize_node(json& j, const MidiInNode& node) const
{
    j = json{
        {"type",       "midi_in"               },
        {"input_name", node.m_input_info.m_name}
    };
}

void NodeSerializer::serialize_node(json& j, const DisconnectedMidiInNode& node) const
{
    j = json{
        {"type",       "midi_in"        },
        {"input_name", node.m_input_name}
    };
}

void NodeSerializer::serialize_node(json& j, const MidiOutNode& node) const
{
    j = json{
        {"type",        "midi_out"               },
        {"output_name", node.m_output_info.m_name}
    };
}

void NodeSerializer::serialize_node(json& j, const DisconnectedMidiOutNode& node) const
{
    j = json{
        {"type",        "midi_out"        },
        {"output_name", node.m_output_name}
    };
}

void NodeSerializer::serialize_node(json& j, const MidiChannelNode& node) const
{
    j = json{
        {"type",     "midi_channel" },
        {"channels", node.m_channels}
    };
}

std::shared_ptr<Node> NodeSerializer::deserialize_node(const json& j) const
{
    const auto            node_type = j.at("type").get<std::string>();
    std::shared_ptr<Node> node;
    if (node_type == "midi_in")
    {
        const auto input_name     = j.at("input_name").get<std::string>();
        const auto input_info_opt = midi::MidiProbe::get_valid_input(input_name);
        if (input_info_opt.has_value())
        {
            node = m_node_factory->build_midi_node(input_info_opt.value());
        }
        else
        {
            node = m_node_factory->build_disconnected_midi_in_node(input_name);
        }
    }
    else if (node_type == "midi_out")
    {
        const auto output_name     = j.at("output_name").get<std::string>();
        const auto output_info_opt = midi::MidiProbe::get_valid_output(output_name);
        if (output_info_opt.has_value())
        {
            node = m_node_factory->build_midi_node(output_info_opt.value());
        }
        else
        {
            node = m_node_factory->build_disconnected_midi_out_node(output_name);
        }
    }
    else if (node_type == "midi_channel")
    {
        auto channel_node = m_node_factory->build_midi_channel_node();
        j.at("channels").get_to(channel_node->m_channels);
        node = channel_node;
    }
    else
    {
        throw std::logic_error("Unexpected node type");
    }
    const auto current_id = node->m_id = j.at("id");
    Node::sm_next_id = std::max(current_id + 1, Node::sm_next_id); // Debt, writing to global state
    ImNodes::SetNodeGridSpacePos(node->id(), j.at("position"));
    return node;
}

} // namespace mc