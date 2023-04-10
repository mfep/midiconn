#include "NodeFactory.hpp"

#include "DisconnectedMidiInNode.hpp"
#include "DisconnectedMidiOutNode.hpp"
#include "MidiChannelNode.hpp"
#include "MidiEngine.hpp"
#include "MidiInNode.hpp"
#include "MidiOutNode.hpp"
#include "Theme.hpp"

namespace mc
{

NodeFactory::NodeFactory(midi::Engine&          midi_engine,
                         const ThemeControl&    theme_control,
                         const PortNameDisplay& port_name_display)
    : m_midi_engine(&midi_engine), m_theme_control(&theme_control),
      m_port_name_display(&port_name_display)
{
}

std::shared_ptr<MidiInNode> NodeFactory::build_midi_node(const midi::InputInfo& input_info) const
{
    return std::make_shared<MidiInNode>(input_info, *m_midi_engine, *m_port_name_display);
}

std::shared_ptr<MidiOutNode> NodeFactory::build_midi_node(
    const midi::OutputInfo& output_info) const
{
    return std::make_shared<MidiOutNode>(output_info, *m_midi_engine, *m_port_name_display);
}

std::shared_ptr<DisconnectedMidiInNode> NodeFactory::build_disconnected_midi_in_node(
    const std::string& input_name) const
{
    return std::make_shared<DisconnectedMidiInNode>(input_name, *m_port_name_display);
}

std::shared_ptr<DisconnectedMidiOutNode> NodeFactory::build_disconnected_midi_out_node(
    const std::string& output_name) const
{
    return std::make_shared<DisconnectedMidiOutNode>(output_name, *m_port_name_display);
}

std::shared_ptr<MidiChannelNode> NodeFactory::build_midi_channel_node() const
{
    return std::make_shared<MidiChannelNode>([this]() {
        return m_theme_control->get_scale_value();
    });
}

} // namespace mc
