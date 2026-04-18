#include "MidiChannelNode.hpp"

#include <iomanip>
#include <numeric>
#include <sstream>

#include "imgui.h"
#include "imnodes.h"

#include "IconsForkAwesome.h"

#include "NodeSerializer.hpp"
#include "ScaleProvider.hpp"

const char* mc::MidiChannelNode::sm_combo_items[] = {
    "None", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16"};

mc::MidiChannelNode::MidiChannelNode(const ScaleProvider&                  scale_provider,
                                     std::shared_ptr<midi::ChannelMapNode> midi_channel_map_node)
    : Node(scale_provider, midi_channel_map_node.get()),
      m_midi_channel_map_node(midi_channel_map_node)
{
}

void mc::MidiChannelNode::accept_serializer(nlohmann::json&       j,
                                            const NodeSerializer& serializer) const
{
    serializer.serialize_node(j, *this);
}

const char* mc::MidiChannelNode::name()
{
    return "Channel map";
}

void mc::MidiChannelNode::render_internal()
{
    Node::begin_input_attribute();
    ImGui::TextUnformatted(name());
    Node::end_input_attribute();
    ImGui::SameLine();
    Node::begin_output_attribute();
    Node::end_output_attribute();
}

void mc::MidiChannelNode::render_inspector()
{
    ImGui::SeparatorText(name());

    std::array<int, midi::ChannelMap::num_channels> channels;
    {
        std::size_t idx = 0;
        for (auto& channel_item : channels)
        {
            const auto channel = m_midi_channel_map_node->map().get(idx++);
            if (channel == midi::ChannelMap::no_channel)
            {
                channel_item = 0;
            }
            else
            {
                channel_item = channel + 1;
            }
        }
    }
    auto updated_channels = channels;

    if (ImGui::Button("Default", {-1, 0}))
    {
        std::iota(updated_channels.begin(), updated_channels.end(), 1);
    }
    if (ImGui::Button("Disable all", {-1, 0}))
    {
        std::fill(updated_channels.begin(), updated_channels.end(), 0);
    }

    if (ImGui::BeginTable("MIDI Channel table", 6, ImGuiTableFlags_SizingStretchProp, {-1, 0}))
    {
        for (size_t i = 0; i < 8; i++)
        {
            ImGui::TableNextRow();
            for (size_t j = 0; j < 2; j++)
            {
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(30 * m_scale_provider->get_scale_value());
                ImGui::TextUnformatted(get_label(i * 2 + j));
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(ICON_FK_LONG_ARROW_RIGHT);
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::Combo(get_hidden_label(i * 2 + j),
                             updated_channels.data() + i * 2 + j,
                             sm_combo_items,
                             sm_num_combo_items);
            }
        }
        ImGui::EndTable();
    }

    if (channels != updated_channels)
    {
        std::size_t idx = 0;
        for (const auto& channel_item : updated_channels)
        {
            if (channel_item == 0)
            {
                m_midi_channel_map_node->map().set(idx, midi::ChannelMap::no_channel);
            }
            else
            {
                m_midi_channel_map_node->map().set(idx, channel_item - 1);
            }
            ++idx;
        }
    }
}

const char* mc::MidiChannelNode::get_label(size_t index)
{
    static std::vector<std::string> labels;
    if (labels.empty())
    {
        for (size_t i = 0; i < sm_num_combo_items; i++)
        {
            std::stringstream ss;
            ss /*<< std::setw(2)*/ << i + 1;
            labels.push_back(ss.str());
        }
    }
    return labels.at(index).c_str();
}

const char* mc::MidiChannelNode::get_hidden_label(size_t index)
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
