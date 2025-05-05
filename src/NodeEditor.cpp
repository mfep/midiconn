#include "NodeEditor.hpp"

#include <algorithm>
#include <string_view>

#include "imgui.h"
#include "imnodes.h"
#include "nlohmann/json.hpp"

#include "LogNode.hpp"
#include "MidiChannelNode.hpp"
#include "MidiInNode.hpp"
#include "MidiOutNode.hpp"
#include "NodeFactory.hpp"
#include "NodeSerializer.hpp"
#include "PortNameDisplay.hpp"
#include "Theme.hpp"
#include "midi/MidiProbe.hpp"

#include "libintl.h"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ImVec2, x, y);

namespace
{
constexpr std::string_view contex_popup_name = "NodeEditorContextMenu";
}

namespace mc
{
namespace
{

template <class MidiInfo>
std::vector<MidiInfo> filter_to_non_instantiated_midi_infos(
    NodeFactory& factory, const std::vector<MidiInfo>& unfiltered_infos)
{
    std::vector<MidiInfo> ret;
    std::copy_if(unfiltered_infos.begin(),
                 unfiltered_infos.end(),
                 std::back_inserter(ret),
                 [&](const auto& info) {
                     return !factory.is_node_instantiated(info);
                 });
    return ret;
}

} // namespace

NodeEditor::NodeEditor(NodeFactory&           node_factory,
                       const PortNameDisplay& port_name_display,
                       const ThemeControl&    theme_control,
                       bool                   create_nodes)
    : m_node_factory(&node_factory), m_port_name_display(&port_name_display),
      m_theme_control(&theme_control)
{
    if (create_nodes)
    {
        instantiate_available_inputs_and_outputs();
    }
}

void NodeEditor::render()
{
    ImNodes::BeginNodeEditor();

    // Render right click context menu
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup(contex_popup_name.data());
    }
    renderContextMenu();
    renderNodes();
    if (m_nodes.size() > 1)
    {
        ImNodes::MiniMap();
    }
    renderHelpText();
    ImNodes::EndNodeEditor();

    handleLinkDropped();
    handleDelete();
    handleConnect();
}

void NodeEditor::to_json(nlohmann::json& j) const
{
    using nlohmann::json;

    auto           node_array = json::array();
    NodeSerializer serializer(*m_node_factory);
    for (const auto& node : m_nodes)
    {
        json node_json;
        serializer.serialize_node(node_json, *node);
        node_array.push_back(node_json);
    }
    const ImVec2 panning = ImNodes::EditorContextGetPanning();
    j                    = json{
                           {"nodes",   node_array},
                           {"panning", panning   }
    };
}

NodeEditor NodeEditor::from_json(NodeFactory&           node_factory,
                                 const PortNameDisplay& port_name_display,
                                 const ThemeControl&    theme_control,
                                 const nlohmann::json&  j)
{
    const ImVec2 panning = j.at("panning");
    ImNodes::EditorContextResetPanning(panning);

    NodeEditor     editor(node_factory, port_name_display, theme_control);
    NodeSerializer deserializer(node_factory);

    // 1st iter -> create nodes
    for (const auto& node_json : j.at("nodes"))
    {
        editor.m_nodes.emplace_back(deserializer.deserialize_node(node_json));
    }

    // 2nd iter -> create connections
    for (const auto& node_json : j.at("nodes"))
    {
        const auto source_node = *std::find_if(editor.m_nodes.begin(),
                                               editor.m_nodes.end(),
                                               [node_id = node_json.at("id")](const auto& node) {
                                                   return node->id() == node_id;
                                               });

        for (const int connected_node_id : node_json.at("output_connection_ids"))
        {
            const auto target_node = *std::find_if(editor.m_nodes.begin(),
                                                   editor.m_nodes.end(),
                                                   [connected_node_id](const auto& node) {
                                                       return node->id() == connected_node_id;
                                                   });
            source_node->connect_output(std::weak_ptr(target_node), std::weak_ptr(source_node));
        }
    }

    return editor;
}

std::shared_ptr<Node> NodeEditor::renderContextMenu(bool show_outputting_nodes,
                                                    bool show_inputting_nodes)
{
    auto render_add_node = [this](auto               node_builder,
                                  const std::string& name) -> std::shared_ptr<Node> {
        constexpr ImGuiTreeNodeFlags leaf_flags =
            ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        ImGui::TreeNodeEx(name.c_str(), leaf_flags);
        if (ImGui::IsItemClicked())
        {
            std::shared_ptr<Node> node = node_builder();
            m_nodes.push_back(node);

            ImNodes::SetNodeScreenSpacePos(node->id(), ImGui::GetMousePosOnOpeningCurrentPopup());
            ImGui::CloseCurrentPopup();
            return node;
        }
        else
        {
            return nullptr;
        }
    };

    auto render_midi_inout_list = [this, render_add_node](auto& infos) -> std::shared_ptr<Node> {
        for (const auto& info : infos)
        {
            const std::string port_name = m_port_name_display->get_port_name(info.m_name);
            if (auto node = render_add_node(
                    [this, &info] {
                        return m_node_factory->build_midi_node(info);
                    },
                    port_name);
                node != nullptr)
            {
                return node;
            }
        }
        return nullptr;
    };

    std::shared_ptr<Node> node;
    ImGui::SetNextWindowSizeConstraints({300, 0}, {FLT_MAX, FLT_MAX});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
    if (ImGui::BeginPopup(contex_popup_name.data()))
    {
        if (ImGui::IsWindowAppearing())
        {
            m_input_infos  = filter_to_non_instantiated_midi_infos(*m_node_factory,
                                                                  midi::MidiProbe::get_inputs());
            m_output_infos = filter_to_non_instantiated_midi_infos(*m_node_factory,
                                                                   midi::MidiProbe::get_outputs());
        }
        if (show_outputting_nodes || show_inputting_nodes)
        {
            node = render_add_node(
                [this] {
                    return m_node_factory->build_midi_channel_node();
                },
                MidiChannelNode::name());
        }
        if (!node && show_inputting_nodes)
        {
            node = render_add_node(
                [this] {
                    return m_node_factory->build_log_node();
                },
                LogNode::name());
        }
        // Translators: Context menu entry for listing the MIDI input devices
        if (!node && show_outputting_nodes && ImGui::TreeNode(gettext("MIDI inputs")))
        {
            node = render_midi_inout_list(m_input_infos);
            ImGui::TreePop();
        }
        // Translators: Context menu entry for listing the MIDI output devices
        if (!node && show_inputting_nodes && ImGui::TreeNode(gettext("MIDI outputs")))
        {
            node = render_midi_inout_list(m_output_infos);
            ImGui::TreePop();
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
    return node;
}

void NodeEditor::renderNodes()
{
    for (const auto& node : m_nodes)
    {
        node->render();
    }
}

void NodeEditor::renderHelpText()
{
    // Translators: Help text in the bottom of the node editor
    const std::string_view help_text = gettext("Left click to select. Right click to create nodes. "
                                               "Middle click (or Ctrl + left click) to pan view.");

    const auto text_size           = ImGui::CalcTextSize(help_text.data());
    const auto window_size         = ImGui::GetWindowSize();
    const auto original_cursor_pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos({window_size.x - text_size.x - 3, window_size.y - text_size.y - 3});
    ImGui::BeginDisabled();
    ImGui::TextUnformatted(help_text.data());
    ImGui::EndDisabled();
    ImGui::SetCursorPos(original_cursor_pos);
}

void NodeEditor::handleDelete()
{
    if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace))
    {
        std::vector<int> selected_ids;
        selected_ids.resize(ImNodes::NumSelectedNodes());
        if (!selected_ids.empty())
        {
            ImNodes::GetSelectedNodes(selected_ids.data());
            // get nodes to remove
            m_nodes.erase(std::remove_if(m_nodes.begin(),
                                         m_nodes.end(),
                                         [&selected_ids](const auto& node) {
                                             return selected_ids.cend() !=
                                                    std::find(selected_ids.cbegin(),
                                                              selected_ids.cend(),
                                                              node->id());
                                         }),
                          m_nodes.end());
        }

        selected_ids.clear();
        selected_ids.resize(ImNodes::NumSelectedLinks());
        if (!selected_ids.empty())
        {
            ImNodes::GetSelectedLinks(selected_ids.data());
            for (const auto& node : m_nodes)
            {
                for (int link_id : selected_ids)
                {
                    node->disconnect_output(link_id);
                }
            }
        }
    }
}

void NodeEditor::handleConnect()
{
    int start_attrib_id;
    int end_attrib_id;
    if (ImNodes::IsLinkCreated(&start_attrib_id, &end_attrib_id))
    {
        auto start_node_it =
            std::find_if(m_nodes.begin(), m_nodes.end(), [start_attrib_id](const auto& node) {
                return node->out_id() == start_attrib_id;
            });
        auto end_node_it =
            std::find_if(m_nodes.begin(), m_nodes.end(), [end_attrib_id](const auto& node) {
                return node->in_id() == end_attrib_id;
            });
        (*start_node_it)
            ->connect_output(std::weak_ptr(*end_node_it), std::weak_ptr(*start_node_it));
    }
}

void NodeEditor::handleLinkDropped()
{
    if (ImNodes::IsLinkDropped(&m_dropped_link_id))
    {
        ImGui::OpenPopup(contex_popup_name.data());
    }
    auto new_node =
        renderContextMenu(Node::is_in_id(m_dropped_link_id), Node::is_out_id(m_dropped_link_id));
    if (new_node)
    {
        if (Node::is_out_id(m_dropped_link_id))
        {
            auto start_node_it =
                std::find_if(m_nodes.begin(), m_nodes.end(), [this](const auto& node) {
                    return node->out_id() == m_dropped_link_id;
                });
            (*start_node_it)
                ->connect_output(std::weak_ptr(new_node), std::weak_ptr(*start_node_it));
        }
        else if (Node::is_in_id(m_dropped_link_id))
        {
            auto end_node_it =
                std::find_if(m_nodes.begin(), m_nodes.end(), [this](const auto& node) {
                    return node->in_id() == m_dropped_link_id;
                });
            new_node->connect_output(std::weak_ptr(*end_node_it), std::weak_ptr(new_node));
        }
    }
}

void NodeEditor::instantiate_available_inputs_and_outputs()
{
    const float scale   = m_theme_control->get_scale_value();
    const float y_start = 25 * scale;
    ImVec2      node_pos{300 * scale, y_start};

    const auto instantiate_nodes = [&](const auto& infos) {
        for (const auto& info : infos)
        {
            auto& node = m_nodes.emplace_back(m_node_factory->build_midi_node(info));
            ImNodes::SetNodeGridSpacePos(node->id(), node_pos);
            node_pos.y += 100 * scale;
        }
    };

    instantiate_nodes(midi::MidiProbe::get_inputs());
    node_pos.x += 300 * scale;
    node_pos.y = y_start;
    instantiate_nodes(midi::MidiProbe::get_outputs());
}

} // namespace mc
