#pragma once

#include <memory>
#include <string>

namespace mc
{

namespace midi
{
class Engine;
struct InputInfo;
struct OutputInfo;
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
    NodeFactory(midi::Engine&          midi_engine,
                const ThemeControl&    theme_control,
                const PortNameDisplay& port_name_display);

    std::shared_ptr<MidiInNode>  build_midi_node(const midi::InputInfo& input_info) const;
    std::shared_ptr<MidiOutNode> build_midi_node(const midi::OutputInfo& output_info) const;
    std::shared_ptr<DisconnectedMidiInNode> build_disconnected_midi_in_node(
        const std::string& input_name) const;
    std::shared_ptr<DisconnectedMidiOutNode> build_disconnected_midi_out_node(
        const std::string& output_name) const;
    std::shared_ptr<MidiChannelNode> build_midi_channel_node() const;

private:
    midi::Engine*          m_midi_engine;
    const ThemeControl*    m_theme_control;
    const PortNameDisplay* m_port_name_display;
};

} // namespace mc
