#pragma once

#include <map>
#include <memory>
#include <string>

namespace mc
{

namespace midi
{
struct InputInfo;
class InputNode;
struct OutputInfo;
class OutputNode;
} // namespace midi

class DisconnectedMidiInNode;
class DisconnectedMidiOutNode;
class MidiChannelNode;
class MidiInNode;
class MidiOutNode;
class PortNameDisplay;
class ThemeControl;

class NodeFactory final
{
public:
    NodeFactory(const ThemeControl& theme_control, const PortNameDisplay& port_name_display);

    std::shared_ptr<MidiInNode>             build_midi_node(const midi::InputInfo& input_info);
    std::shared_ptr<MidiOutNode>            build_midi_node(const midi::OutputInfo& output_info);
    std::shared_ptr<DisconnectedMidiInNode> build_disconnected_midi_in_node(
        const std::string& input_name);
    std::shared_ptr<DisconnectedMidiOutNode> build_disconnected_midi_out_node(
        const std::string& output_name);
    std::shared_ptr<MidiChannelNode> build_midi_channel_node();

    bool is_node_instantiated(const midi::InputInfo& input_info);
    bool is_node_instantiated(const midi::OutputInfo& output_info);

private:
    const ThemeControl*    m_theme_control;
    const PortNameDisplay* m_port_name_display;
    std::map<std::size_t, std::weak_ptr<midi::InputNode>>  m_input_nodes;
    std::map<std::size_t, std::weak_ptr<midi::OutputNode>> m_output_nodes;
};

} // namespace mc
