#include "MidiChannelNode.hpp"

#include <iomanip>
#include <numeric>
#include <sstream>

#include "imgui.h"
#include "imnodes.h"

#include "NodeSerializer.hpp"

#include "libintl.h"

const char* mc::MidiChannelNode::sm_combo_items[] = {gettext("None"),
                                                     "1",
                                                     "2",
                                                     "3",
                                                     "4",
                                                     "5",
                                                     "6",
                                                     "7",
                                                     "8",
                                                     "9",
                                                     "10",
                                                     "11",
                                                     "12",
                                                     "13",
                                                     "14",
                                                     "15",
                                                     "16"};

mc::MidiChannelNode::MidiChannelNode(const ScaleProvider& scale_provider)
    : m_scale_provider(&scale_provider)
{
    m_midi_channel_map_node.add_observer(this);
}

mc::MidiChannelNode::~MidiChannelNode()
{
    m_midi_channel_map_node.remove_observer(this);
}

void mc::MidiChannelNode::accept_serializer(nlohmann::json&       j,
                                            const NodeSerializer& serializer) const
{
    serializer.serialize_node(j, *this);
}

const char* mc::MidiChannelNode::name()
{
    // Translators: The name of the channel map node
    return gettext("Channel map");
}

mc::midi::Node* mc::MidiChannelNode::get_midi_node()
{
    return &m_midi_channel_map_node;
}

void mc::MidiChannelNode::render_internal()
{
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(name());
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginInputAttribute(in_id());
    m_input_indicator.render();
    ImGui::SameLine();
    ImGui::TextUnformatted(gettext("MIDI in"));
    ImNodes::EndInputAttribute();
    ImGui::SameLine(100 * m_scale_provider->get_scale_value());
    ImNodes::BeginOutputAttribute(out_id());
    ImGui::TextUnformatted(gettext("MIDI out"));
    ImGui::SameLine();
    m_output_indicator.render();
    ImNodes::EndOutputAttribute();

    std::array<int, midi::ChannelMap::num_channels> channels;
    {
        std::size_t idx = 0;
        for (auto& channel_item : channels)
        {
            const auto channel = m_midi_channel_map_node.map().get(idx++);
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

    if (ImGui::BeginTable("MIDI Channel table",
                          4,
                          ImGuiTableFlags_SizingStretchProp,
                          {160 * m_scale_provider->get_scale_value(), 0}))
    {
        for (size_t i = 0; i < 8; i++)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(get_label(i * 2));
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(50 * m_scale_provider->get_scale_value());
            ImGui::Combo(get_hidden_label(i * 2),
                         updated_channels.data() + i * 2,
                         sm_combo_items,
                         sm_num_combo_items);

            ImGui::TableNextColumn();
            ImGui::TextUnformatted(get_label(i * 2 + 1));
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(50 * m_scale_provider->get_scale_value());
            ImGui::Combo(get_hidden_label(i * 2 + 1),
                         updated_channels.data() + i * 2 + 1,
                         sm_combo_items,
                         sm_num_combo_items);
        }
        ImGui::EndTable();
    }
    // Translators: Button to set the default channel map
    if (ImGui::Button(gettext("Default")))
    {
        std::iota(updated_channels.begin(), updated_channels.end(), 1);
    }
    ImGui::SameLine();
    // Translators: Button to disable all channels in the channel map
    if (ImGui::Button(gettext("Disable all")))
    {
        std::fill(updated_channels.begin(), updated_channels.end(), 0);
    }

    if (channels != updated_channels)
    {
        midi::ChannelMap new_map;
        std::size_t      idx = 0;
        for (const auto& channel_item : updated_channels)
        {
            if (channel_item == 0)
            {
                m_midi_channel_map_node.map().set(idx, midi::ChannelMap::no_channel);
            }
            else
            {
                m_midi_channel_map_node.map().set(idx, channel_item - 1);
            }
            ++idx;
        }
    }
}

void mc::MidiChannelNode::message_processed(std::span<const unsigned char> /*message_bytes*/)
{
    m_output_indicator.trigger();
}

void mc::MidiChannelNode::message_received(std::span<const unsigned char> /*message_bytes*/)
{
    m_input_indicator.trigger();
}

const char* mc::MidiChannelNode::get_label(size_t index)
{
    static std::vector<std::string> labels;
    if (labels.empty())
    {
        for (size_t i = 0; i < sm_num_combo_items; i++)
        {
            std::stringstream ss;
            ss << std::setw(2) << i + 1;
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
