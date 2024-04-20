#include "NodeFactory.hpp"

#include "DisconnectedMidiInNode.hpp"
#include "DisconnectedMidiOutNode.hpp"
#include "MidiChannelNode.hpp"
#include "MidiInNode.hpp"
#include "MidiOutNode.hpp"
#include "Theme.hpp"
#include "midi/InputNode.hpp"
#include "midi/OutputNode.hpp"

namespace mc
{
namespace
{

template <class MidiNode, class MidiInfo>
std::shared_ptr<MidiNode> get_cached_midi_node(
    std::map<std::size_t, std::weak_ptr<MidiNode>>& node_map, const MidiInfo& info)
{
    std::shared_ptr<MidiNode> midi_node_ptr;
    auto                      found_it = node_map.find(info.m_id);
    if (found_it == node_map.end())
    {
        return nullptr;
    }
    else if (auto midi_node_ptr = found_it->second.lock(); midi_node_ptr == nullptr)
    {
        node_map.erase(found_it);
        return nullptr;
    }
    else
    {
        return midi_node_ptr;
    }
}

template <class MidiNode, class MidiInfo>
std::shared_ptr<MidiNode> make_cached_midi_node(
    std::map<std::size_t, std::weak_ptr<MidiNode>>& node_map, const MidiInfo& info)
{
    if (auto midi_node_ptr = get_cached_midi_node(node_map, info); midi_node_ptr == nullptr)
    {
        midi_node_ptr = std::make_shared<MidiNode>(info);
        node_map.emplace(std::make_pair(info.m_id, midi_node_ptr));
        return midi_node_ptr;
    }
    else
    {
        return midi_node_ptr;
    }
}

} // namespace

NodeFactory::NodeFactory(const ThemeControl&    theme_control,
                         const PortNameDisplay& port_name_display)
    : m_theme_control(&theme_control), m_port_name_display(&port_name_display)
{
}

std::shared_ptr<MidiInNode> NodeFactory::build_midi_node(const midi::InputInfo& input_info)
{
    return std::make_shared<MidiInNode>(
        input_info, make_cached_midi_node(m_input_nodes, input_info), *m_port_name_display);
}

std::shared_ptr<MidiOutNode> NodeFactory::build_midi_node(const midi::OutputInfo& output_info)
{
    return std::make_shared<MidiOutNode>(
        output_info, make_cached_midi_node(m_output_nodes, output_info), *m_port_name_display);
}

std::shared_ptr<DisconnectedMidiInNode> NodeFactory::build_disconnected_midi_in_node(
    const std::string& input_name)
{
    return std::make_shared<DisconnectedMidiInNode>(input_name, *m_port_name_display);
}

std::shared_ptr<DisconnectedMidiOutNode> NodeFactory::build_disconnected_midi_out_node(
    const std::string& output_name)
{
    return std::make_shared<DisconnectedMidiOutNode>(output_name, *m_port_name_display);
}

std::shared_ptr<MidiChannelNode> NodeFactory::build_midi_channel_node()
{
    return std::make_shared<MidiChannelNode>([this]() {
        return m_theme_control->get_scale_value();
    });
}

bool NodeFactory::is_node_instantiated(const midi::InputInfo& input_info)
{
    return nullptr != get_cached_midi_node(m_input_nodes, input_info);
}

bool NodeFactory::is_node_instantiated(const midi::OutputInfo& output_info)
{
    return nullptr != get_cached_midi_node(m_output_nodes, output_info);
}

} // namespace mc
