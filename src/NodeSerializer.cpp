#include "NodeSerializer.hpp"
#include <nlohmann/json.hpp>
#include "MidiInfo.hpp"
#include "MidiChannelNode.hpp"
#include "MidiInNode.hpp"
#include "MidiOutNode.hpp"

namespace mc
{
namespace midi
{

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InputInfo, m_id, m_name);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(OutputInfo, m_id, m_name);

}

using nlohmann::json;

NodeSerializer::NodeSerializer(midi::Engine& midi_engine) :
    m_midi_engine(midi_engine)
{
}

void NodeSerializer::serialize_node(json& j, const Node& node) const
{
    node.accept_serializer(j, *this);
}

void NodeSerializer::serialize_node(json& j, const MidiInNode& node) const
{
    j = json{ { "type", "midi_in" }, { "input_info", node.m_input_info } };
}

void NodeSerializer::serialize_node(json& j, const MidiOutNode& node) const
{
    j = json{ { "type", "midi_out" }, { "output_info", node.m_output_info } };
}

void NodeSerializer::serialize_node(json& j, const MidiChannelNode& node) const
{
    j = json{ { "type", "midi_channel" }, { "channels", node.m_channels } };
}

std::shared_ptr<Node> NodeSerializer::deserialize_node(const json& j) const
{
    const auto node_type = j["type"].get<std::string>();
    if (node_type == "midi_in")
    {
        const auto input_info = j["input_info"].get<midi::InputInfo>();
        return std::make_shared<MidiInNode>(input_info, m_midi_engine);
    }
    else if (node_type == "midi_out")
    {
        const auto output_info = j["output_info"].get<midi::OutputInfo>();
        return std::make_shared<MidiOutNode>(output_info, m_midi_engine);
    }
    else if (node_type == "midi_channel")
    {
        auto channel_node = std::make_shared<MidiChannelNode>();
        j["channels"].get_to(channel_node->m_channels);
        return channel_node;
    }
    throw std::exception();
}

}