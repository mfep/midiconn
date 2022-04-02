#include "MidiChannelNode.hpp"
#include <iomanip>
#include <numeric>
#include <sstream>
#include "imgui.h"
#include "imnodes.h"
#include "NodeSerializer.hpp"

namespace mc
{

const char* MidiChannelNode::sm_combo_items[] = {
    "None", "1", "2", "3", "4", "5", "6", "7", "8",
    "9", "10", "11", "12", "13", "14", "15", "16"
};

MidiChannelNode::MidiChannelNode()
{
    std::iota(m_channels.begin(), m_channels.end(), 1);
}

void MidiChannelNode::accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const
{
    serializer.serialize_node(j, *this);
}

void MidiChannelNode::render_internal()
{
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Channel map");
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginInputAttribute(in_id());
    ImGui::TextUnformatted("MIDI in");
    ImNodes::EndInputAttribute();
    ImGui::SameLine(135);
    ImNodes::BeginOutputAttribute(out_id());
    ImGui::TextUnformatted("MIDI out");
    ImNodes::EndOutputAttribute();

    const auto previous_channels = m_channels;
    for (size_t i = 0; i < 8; i++)
    {
        ImGui::TextUnformatted(get_label(i * 2));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        ImGui::Combo(get_hidden_label(i * 2), m_channels.data() + i * 2,
                     sm_combo_items, sm_num_combo_items);

        ImGui::SameLine();
        ImGui::TextUnformatted(get_label(i * 2 + 1));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        ImGui::Combo(get_hidden_label(i * 2 + 1), m_channels.data() + i * 2 + 1,
                     sm_combo_items, sm_num_combo_items);
    }
    if (ImGui::Button("Default"))
    {
        std::iota(m_channels.begin(), m_channels.end(), 1);
    }
    ImGui::SameLine();
    if (ImGui::Button("Disable all"))
    {
        std::fill(m_channels.begin(), m_channels.end(), 0);
    }
    if (m_channels != previous_channels)
    {
        update_outputs_with_sources();
    }
}

midi::channel_map MidiChannelNode::transform_channel_map(const midi::channel_map& in_map)
{
    midi::channel_map out_map;
    for (size_t channel_input_index = 0; channel_input_index < in_map.size(); channel_input_index++)
    {
        const auto channel_output_value = m_channels[channel_input_index];
        const auto channel_output_index = static_cast<char>(channel_output_value - 1);
        for (size_t i = 0; i < in_map.size(); i++)
        {
            if (in_map[i] == channel_input_index)
            {
                out_map[i] = channel_output_index;
            }
        }
    }
    return out_map;
}

const char* MidiChannelNode::get_label(size_t index)
{
    static std::vector<std::string> labels;
    if (labels.empty())
    {
        for (size_t i = 0; i < sm_num_combo_items; i++)
        {
            std::stringstream ss;
            ss << std::setw(2) << i + 1 << " ->";
            labels.push_back(ss.str());
        }
    }
    return labels.at(index).c_str();
}

const char* MidiChannelNode::get_hidden_label(size_t index)
{
    static std::vector<std::string> labels;
    if (labels.empty())
    {
        for (size_t i = 0; i < sm_num_combo_items; i++)
        {
            labels.push_back("##" + std::string(get_label(i)));
        }
    }
    return labels.at(index).c_str();
}

}
