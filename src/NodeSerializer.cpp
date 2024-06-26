#include "NodeSerializer.hpp"

#include <optional>

#include "imnodes.h"
#include "nlohmann/json.hpp"

#include "LogNode.hpp"
#include "MidiChannelNode.hpp"
#include "MidiInNode.hpp"
#include "MidiOutNode.hpp"
#include "NodeFactory.hpp"
#include "midi/MessageTypeMask.hpp"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ImVec2, x, y);

namespace mc::midi
{
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MessageTypeMask,
                                   m_sysex_enabled,
                                   m_time_enabled,
                                   m_sensing_enabled);
}

namespace mc
{

using nlohmann::json;

NodeSerializer::NodeSerializer(NodeFactory& node_factory) : m_node_factory(&node_factory)
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
        {"type",              "midi_in"               },
        {"input_name",        node.m_input_name       },
        {"message_type_mask", node.m_message_type_mask},
    };
}

void NodeSerializer::serialize_node(json& j, const MidiOutNode& node) const
{
    j = json{
        {"type",        "midi_out"        },
        {"output_name", node.m_output_name}
    };
}

void NodeSerializer::serialize_node(nlohmann::json& j, const LogNode& node) const
{
    j = json{
        {"type",            "log"                 },
        {"max_buffer_size", node.m_max_buffer_size},
    };
}

void NodeSerializer::serialize_node(json& j, const MidiChannelNode& node) const
{
    j = json{
        {"type",     "midi_channel"                           },
        {"channels", node.m_midi_channel_map_node.map().data()}
    };
}

std::shared_ptr<Node> NodeSerializer::deserialize_node(const json& j) const
{
    const auto            node_type = j.at("type").get<std::string>();
    std::shared_ptr<Node> node;
    if (node_type == "midi_in")
    {
        const auto input_name   = j.at("input_name").get<std::string>();
        auto       midi_in_node = m_node_factory->build_midi_in_node(input_name);
        if (j.contains("message_type_mask"))
        {
            midi_in_node->set_message_type_mask(
                j.at("message_type_mask").get<midi::MessageTypeMask>());
        }
        node = midi_in_node;
    }
    else if (node_type == "midi_out")
    {
        const auto output_name = j.at("output_name").get<std::string>();
        node                   = m_node_factory->build_midi_out_node(output_name);
    }
    else if (node_type == "midi_channel")
    {
        auto channel_node = m_node_factory->build_midi_channel_node();
        channel_node->m_midi_channel_map_node.map().data() =
            j.at("channels").get<midi::ChannelMap::data_t>();
        node = channel_node;
    }
    else if (node_type == "log")
    {
        auto log_node = m_node_factory->build_log_node();
        j["max_buffer_size"].get_to(log_node->m_max_buffer_size);
        node = log_node;
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