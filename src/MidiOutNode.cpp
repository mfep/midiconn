#include "MidiOutNode.hpp"
#include "imgui.h"
#include "imnodes.h"
#include "MidiEngine.hpp"

namespace mc
{

MidiOutNode::MidiOutNode(const midi::OutputInfo& output_info, midi::Engine& midi_engine) :
    m_output_info(output_info),
    m_midi_engine(midi_engine)
{
    midi_engine.create(output_info);
}

MidiOutNode::~MidiOutNode()
{
    m_midi_engine.remove(m_output_info);
}

void MidiOutNode::render_internal()
{
    imnodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(m_output_info.m_name.c_str());
    imnodes::EndNodeTitleBar();
    imnodes::BeginInputAttribute(in_id());
    ImGui::TextUnformatted("all channels");
    imnodes::EndInputAttribute();
}

void MidiOutNode::update_outputs_with_sources()
{
    for (auto&[id, map] : m_previous_sources)
    {
        m_midi_engine.disconnect(id, m_output_info.m_id);
    }
    for (auto&[id, map] : m_sources)
    {
        m_midi_engine.connect(id, m_output_info.m_id, map);
    }
    m_previous_sources = m_sources;
}

}
