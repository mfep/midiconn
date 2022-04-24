#include "MidiPortWatchdog.hpp"

#include "DisconnectedMidiInNode.hpp"
#include "DisconnectedMidiOutNode.hpp"
#include "MidiInNode.hpp"
#include "MidiOutNode.hpp"
#include "MidiProbe.hpp"

namespace mc
{
namespace
{

template<class MidiNode>
struct MidiNodeTraits;

template<>
struct MidiNodeTraits<MidiInNode>
{
    static constexpr bool is_disconnected = false;
};

template<>
struct MidiNodeTraits<MidiOutNode>
{
    static constexpr bool is_disconnected = false;
};

template<>
struct MidiNodeTraits<DisconnectedMidiInNode>
{
    static constexpr bool is_disconnected = true;
};

template<>
struct MidiNodeTraits<DisconnectedMidiOutNode>
{
    static constexpr bool is_disconnected = true;
};

template<class MidiNode, class ReplacingMidiNode, class ValidGetterFun, class ... Args>
bool update_node(std::shared_ptr<Node>& node_ptr, ValidGetterFun valid_getter_fun, Args&& ... args)
{
    if (auto* midi_ptr = dynamic_cast<MidiNode*>(node_ptr.get()); midi_ptr != nullptr)
    {
        std::string midi_port_name;
        if constexpr (MidiNodeTraits<MidiNode>::is_disconnected)
        {
            midi_port_name = midi_ptr->get_name();
        }
        else
        {
            midi_port_name = midi_ptr->get_info().m_name;
        }

        const auto valid_info = valid_getter_fun(midi_port_name);

        if constexpr (MidiNodeTraits<MidiNode>::is_disconnected)
        {
            if (valid_info.has_value())
            {
                node_ptr = std::make_shared<ReplacingMidiNode>(valid_info.value(), std::forward<Args>(args)...);
            }
        }
        else
        {
            if (!valid_info.has_value() || valid_info.value().m_id != midi_ptr->get_info().m_id)
            {
                node_ptr = std::make_shared<ReplacingMidiNode>(midi_port_name);
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

}   // namespace

void MidiPortWatchdog::check_nodes(std::vector<std::shared_ptr<Node>>& nodes, midi::Engine& midi_engine)
{
    for (auto& node_ptr : nodes)
    {
        const auto get_valid_input = [](const auto& name) { return MidiProbe::get_valid_input(name); };
        const auto get_valid_output = [](const auto& name) { return MidiProbe::get_valid_output(name); };

        if (update_node<MidiInNode, DisconnectedMidiInNode>(node_ptr, get_valid_input))
        {
            // do nothing, update done in side effect
        }
        else if (update_node<MidiOutNode, DisconnectedMidiOutNode>(node_ptr, get_valid_output))
        {
            // do nothing, update done in side effect
        }
        else if (update_node<DisconnectedMidiInNode, MidiInNode>(node_ptr, get_valid_input, midi_engine))
        {
            // do nothing, update done in side effect
        }
        else if (update_node<DisconnectedMidiOutNode, MidiOutNode>(node_ptr, get_valid_output, midi_engine))
        {
            // do nothing, update done in side effect
        }
    }
}

}