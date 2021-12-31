#include "MidiNodeObserver.hpp"

namespace mc
{

MidiNodeObserver::MidiNodeObserver(midi::Engine* midi_engine) :
    m_midi_engine(midi_engine)
{
}

void MidiNodeObserver::node_created(const midi::InputInfo& input_info)
{
    m_midi_engine->create(input_info);
}

void MidiNodeObserver::node_created(const midi::OutputInfo& output_info)
{
    m_midi_engine->create(output_info);
}

void MidiNodeObserver::node_destroyed(const midi::InputInfo& input_info)
{
    m_midi_engine->remove(input_info);
}

void MidiNodeObserver::node_destroyed(const midi::OutputInfo& output_info)
{
    m_midi_engine->remove(output_info);
}

void MidiNodeObserver::link_created(const midi::InputInfo& input_info, const midi::OutputInfo& output_info)
{
    m_midi_engine->connect(input_info, output_info);
}

void MidiNodeObserver::link_destroyed(const midi::InputInfo& input_info, const midi::OutputInfo& output_info)
{
    m_midi_engine->disconnect(input_info, output_info);
}

}
