#include "MidiInNode.hpp"
#include <algorithm>
#include <numeric>
#include "imgui.h"
#include "imnodes.h"
#include "MidiEngine.hpp"

namespace mc
{

MidiInNode::MidiInNode(const midi::InputInfo& input_info, midi::Engine& midi_engine) :
    m_input_info(input_info),
    m_midi_engine(midi_engine)
{
    midi_engine.create(input_info);
    auto& map = m_sources[input_info.m_id] = {};
    std::iota(map.begin(), map.end(), 0ull);
}

MidiInNode::~MidiInNode()
{
    // ToDo
    m_midi_engine.remove(m_input_info);
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

}
