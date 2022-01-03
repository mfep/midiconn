#include "MidiOutNode.hpp"

#include "imgui.h"
#include "imnodes.h"

namespace mc
{

MidiOutNode::MidiOutNode(const midi::OutputInfo& output_info) :
    m_output_info(output_info)
{
    m_midi_output.openPort(m_output_info.m_id);
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

void MidiOutNode::process_internal(size_t size, const_data_itr in_itr, data_itr out_itr)
{
    m_midi_output.sendMessage(const_cast<unsigned char*>(&*in_itr), size);
}

}
