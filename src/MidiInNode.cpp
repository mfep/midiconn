#include "MidiInNode.hpp"
#include <algorithm>
#include "imgui.h"
#include "imnodes.h"

namespace mc
{

MidiInNode::MidiInNode(const midi::InputInfo& input_info) :
    m_input_info(input_info)
{
    m_midi_input.setCallback(handle_input, this);
    m_midi_input.openPort(input_info.m_id);
}

void MidiInNode::render_internal()
{
    imnodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(m_input_info.m_name.c_str());
    imnodes::EndNodeTitleBar();
    imnodes::BeginOutputAttribute(out_id());
    ImGui::TextUnformatted("all channels");
    imnodes::EndOutputAttribute();
}

void MidiInNode::process_internal(size_t size, const_data_itr in_itr, data_itr out_itr)
{
    std::copy_n(in_itr, size, out_itr);
}

void MidiInNode::handle_input(double /*timeStamp*/, std::vector<unsigned char>* message, void* data)
{
    auto* instance = static_cast<MidiInNode*>(data);
    instance->process(message->size(), message->cbegin());
}

}
