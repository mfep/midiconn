#include "LogNode.hpp"

#include "NodeSerializer.hpp"
#include "Intl.hpp"

#include "midi/MessageView.hpp"
#include "midi/Note.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "fmt/format.h"

std::string_view mc::LogNode::LogMidiNode::name()
{
    return "Log Node";
}

mc::LogNode::LogNode(const ScaleProvider& scale_provider) : m_scale_provider(&scale_provider)
{
    m_log_midi_node.add_observer(this);
}

mc::LogNode::~LogNode()
{
    m_log_midi_node.remove_observer(this);
}

void mc::LogNode::accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const
{
    serializer.serialize_node(j, *this);
}

const char* mc::LogNode::name()
{
    // Translators: The name of the log node
    return gettext("Message log");
}

mc::midi::Node* mc::LogNode::get_midi_node()
{
    return &m_log_midi_node;
}

void mc::LogNode::render_internal()
{
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(name());
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginInputAttribute(in_id());
    m_input_indicator.render();
    ImGui::SameLine();
    // Translators: The caption of the MIDI input pin on the node
    ImGui::TextUnformatted(gettext("MIDI in"));
    ImNodes::EndInputAttribute();

    bool clear = false;
    // Translators: The button to clear the buffer of the log node
    if (ImGui::Button(gettext("Clear")))
    {
        clear = true;
    }
    ImGui::SameLine();
    int new_buffer_size = static_cast<int>(m_max_buffer_size);
    ImGui::SetNextItemWidth(100.0F * m_scale_provider->get_scale_value());
    // Translators: The input field to set the buffer size of the log node
    if (ImGui::InputInt(gettext("Buffer Size"), &new_buffer_size))
    {
        if (new_buffer_size > 0 && new_buffer_size < 100000)
        {
            m_max_buffer_size = static_cast<std::size_t>(new_buffer_size);
        }
    }

    std::unique_lock lock(m_buffer_mutex);
    if (clear)
    {
        m_message_buffer.clear();
    }
    while (!m_message_buffer.empty() && m_message_buffer.size() > m_max_buffer_size)
    {
        m_message_buffer.pop_back();
    }
    lock.unlock();

    if (ImGui::BeginTable("Log",
                          5,
                          ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable,
                          ImVec2{530.F * m_scale_provider->get_scale_value(),
                                 200.F * m_scale_provider->get_scale_value()}))
    {
        // Translators: The header of the message arrive time column
        ImGui::TableSetupColumn(gettext("Time"),
                                ImGuiTableColumnFlags_WidthFixed,
                                100.F * m_scale_provider->get_scale_value());
        // Translators: The header of the message type column
        ImGui::TableSetupColumn(gettext("Type"));
        // Translators: The header of the message channel column
        ImGui::TableSetupColumn(gettext("Channel"),
                                ImGuiTableColumnFlags_WidthFixed,
                                50.F * m_scale_provider->get_scale_value());
        // Translators: The header of the first message data column
        ImGui::TableSetupColumn(gettext("Data #0"));
        // Translators: The header of the second message data column
        ImGui::TableSetupColumn(gettext("Data #1"));
        ImGui::TableHeadersRow();

        lock.lock();
        for (const auto& item : m_message_buffer)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            const auto           arrived = std::chrono::system_clock::to_time_t(item.m_arrived);
            const auto*          local   = std::localtime(&arrived);
            std::array<char, 20> time_str{};
            std::strftime(time_str.data(), time_str.size(), "%T", local);
            ImGui::Text("%s.%03ld",
                        time_str.data(),
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            item.m_arrived.time_since_epoch())
                                .count() %
                            1000);

            ImGui::TableNextColumn();
            ImGui::TextUnformatted(item.m_name.c_str());

            ImGui::TableNextColumn();
            if (item.m_channel)
            {
                ImGui::TextUnformatted(std::to_string(*item.m_channel).c_str());
            }

            ImGui::TableNextColumn();
            if (!item.m_data_0.empty())
            {
                ImGui::TextUnformatted(item.m_data_0.c_str());
            }

            ImGui::TableNextColumn();
            if (!item.m_data_1.empty())
            {
                ImGui::TextUnformatted(item.m_data_1.c_str());
            }
        }
        lock.unlock();

        ImGui::EndTable();
    }
}

void mc::LogNode::message_received(std::span<const unsigned char> message_bytes)
{
    using namespace std::string_literals;

    m_input_indicator.trigger();
    midi::MessageView   message(message_bytes);
    midi::tag_overloads message_visitor{
        [](midi::ChannelMessageViewTag, auto channel_message) -> BufferElement {
            return parse_channel_message(channel_message);
        },
        [](midi::SystemMessageViewTag, auto system_message) -> BufferElement {
            return parse_system_message(system_message);
        },
        []<class Tag>(
            Tag, auto) -> std::enable_if_t<!std::is_base_of_v<midi::ChannelMessageViewTag, Tag> &&
                                               !std::is_base_of_v<midi::SystemMessageViewTag, Tag>,
                                           BufferElement> {
            // Translators: Name of an unrecognized MIDI message
            return BufferElement{gettext("Unknown")};
        }};
    const BufferElement new_element = std::visit(message_visitor, message.parse());

    std::lock_guard guard(m_buffer_mutex);
    m_message_buffer.push_front(std::move(new_element));
}

mc::LogNode::BufferElement mc::LogNode::parse_channel_message(
    midi::ChannelMessageView<false> message_view)
{
    using namespace std::string_literals;
    midi::tag_overloads message_visitor{
        [](midi::NoteOnMessageViewTag, auto note_on) -> BufferElement {
            return BufferElement{
                gettext("Note On"),
                note_on.get_channel_human(),
                fmt::format("{}", midi::Note(note_on.get_note())),
                fmt::format("{}: {}", gettext("Velocity"), note_on.get_velocity())};
        },
        [](midi::NoteOffMessageViewTag, auto note_off) -> BufferElement {
            return BufferElement{
                gettext("Note Off"),
                note_off.get_channel_human(),
                fmt::format("{}", midi::Note(note_off.get_note())),
                fmt::format("{}: {}", gettext("Velocity"), note_off.get_velocity())};
        },
        [](midi::PolyKeyPressureMessageViewTag, auto poly_key_pressure) -> BufferElement {
            return BufferElement{
                gettext("Poly Aftertouch"),
                poly_key_pressure.get_channel_human(),
                fmt::format("{}", midi::Note(poly_key_pressure.get_note())),
                fmt::format("{}: {}", gettext("Pressure"), poly_key_pressure.get_pressure())};
        },
        [](midi::AllSoundOffMessageViewTag, auto all_sound_off) -> BufferElement {
            return BufferElement{gettext("All Sound Off"), all_sound_off.get_channel_human()};
        },
        [](midi::ResetAllControllersMessageViewTag, auto reset_all_controllers) -> BufferElement {
            return BufferElement{gettext("Reset All Controllers"),
                                 reset_all_controllers.get_channel_human()};
        },
        [](midi::LocalControlMessageViewTag, auto local_control) -> BufferElement {
            return BufferElement{gettext("Local Control"),
                                 local_control.get_channel_human(),
                                 local_control.get_value() ? gettext("On") : gettext("Off")};
        },
        [](midi::AllNotesOffMessageViewTag, auto all_notes_off) -> BufferElement {
            return BufferElement{gettext("All Notes Off"), all_notes_off.get_channel_human()};
        },
        [](midi::OmniModeOffMessageViewTag, auto omni_off) -> BufferElement {
            return BufferElement{gettext("Omni Mode Off"), omni_off.get_channel_human()};
        },
        [](midi::OmniModeOnMessageViewTag, auto omni_on) -> BufferElement {
            return BufferElement{gettext("Omni Mode On"), omni_on.get_channel_human()};
        },
        [](midi::MonoModeOnMessageViewTag, auto mono_mode) -> BufferElement {
            return BufferElement{
                gettext("Mono Mode On"),
                mono_mode.get_channel_human(),
                fmt::format("{}: {}", gettext("Channels"), mono_mode.get_num_channels())};
        },
        [](midi::PolyModeOnMessageViewTag, auto poly_mode) -> BufferElement {
            return BufferElement{gettext("Poly Mode On"), poly_mode.get_channel_human()};
        },
        [](midi::ControlChangeMessageViewTag, auto control_change) -> BufferElement {
            return BufferElement{
                gettext("Control Change"),
                control_change.get_channel_human(),
                fmt::format("CC {} ({})",
                            control_change.get_controller(),
                            control_change.get_function_name()),
                fmt::format("{}: {}", gettext("Value"), control_change.get_value())};
        },
        [](midi::ProgramChangeMessageViewTag, auto program_change) -> BufferElement {
            return BufferElement{
                gettext("Program Change"),
                program_change.get_channel_human(),
                fmt::format("{}: {}", gettext("Program"), program_change.get_program_number())};
        },
        [](midi::ChannelPressureMessageViewTag, auto channel_pressure) -> BufferElement {
            return BufferElement{
                gettext("Channel Aftertouch"),
                channel_pressure.get_channel_human(),
                fmt::format("{}: {}", gettext("Pressure"), channel_pressure.get_pressure())};
        },
        [](midi::PitchBendMessageViewTag, auto pitch_bend) -> BufferElement {
            return BufferElement{
                gettext("Pitch bend"),
                pitch_bend.get_channel_human(),
                fmt::format("{}: {}", gettext("Value"), pitch_bend.get_value_human())};
        },
        [](auto, auto) -> BufferElement {
            return BufferElement{gettext("Unknown channel message")};
        }};
    return std::visit(message_visitor, message_view.parse());
}

mc::LogNode::BufferElement mc::LogNode::parse_system_message(
    midi::SystemMessageView<false> message_view)
{
    using namespace std::string_literals;
    midi::tag_overloads message_visitor{
        [](midi::SystemExclusiveMessageViewTag, auto system_exclusive) -> BufferElement {
            return BufferElement{
                gettext("System Exclusive"),
                fmt::format("ID: {}", system_exclusive.get_manufacturer_id()),
                fmt::format("{} {}", system_exclusive.get_length(), gettext("bytes"))};
        },
        [](midi::TimeCodeQuarterFrameMessageViewTag, auto timecode_quarter) -> BufferElement {
            return BufferElement{
                gettext("Timecode Quarter Frame"),
                fmt::format("{}: {}", gettext("Type"), timecode_quarter.get_type()),
                fmt::format("{}: {}", gettext("Values"), timecode_quarter.get_values())};
        },
        [](midi::SongPositionPointerMessageViewTag, auto song_position) -> BufferElement {
            return BufferElement{gettext("Song Position"),
                                 std::to_string(song_position.get_position())};
        },
        [](midi::SongSelectMessageViewTag, auto song_select) -> BufferElement {
            return BufferElement{gettext("Song Select"), std::to_string(song_select.get_song())};
        },
        [](midi::TuneRequestMessageViewTag, auto) -> BufferElement {
            return BufferElement{gettext("Tune Request")};
        },
        [](midi::TimingClockMessageViewTag, auto) -> BufferElement {
            return BufferElement{gettext("Timing Clock")};
        },
        [](midi::StartSequenceMessageViewTag, auto) -> BufferElement {
            return BufferElement{gettext("Start Sequence")};
        },
        [](midi::ContinueSequenceMessageViewTag, auto) -> BufferElement {
            return BufferElement{gettext("Continue Sequence")};
        },
        [](midi::StopSequenceMessageViewTag, auto) -> BufferElement {
            return BufferElement{gettext("Stop Sequence")};
        },
        [](midi::ActiveSensingMessageViewTag, auto) -> BufferElement {
            return BufferElement{gettext("Active Sensing")};
        },
        [](midi::ResetMessageViewTag, auto) -> BufferElement {
            return BufferElement{gettext("Reset everything")};
        },
        [](auto, auto) -> BufferElement {
            return BufferElement{gettext("Unknown system message")};
        }};
    return std::visit(message_visitor, message_view.parse());
}
