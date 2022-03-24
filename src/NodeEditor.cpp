#include "NodeEditor.hpp"
#include <algorithm>
#include <nlohmann/json.hpp>
#include "imgui.h"
#include "imnodes.h"
#include "MidiEngine.hpp"
#include "MidiChannelNode.hpp"
#include "MidiInNode.hpp"
#include "MidiOutNode.hpp"
#include "NodeSerializer.hpp"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ImVec2, x, y);

namespace mc::display
{

NodeEditor::NodeEditor(midi::Engine& midi_engine) :
    m_midi_engine(midi_engine)
{
}

void NodeEditor::render()
{
    ImNodes::BeginNodeEditor();
    renderContextMenu();
    renderNodes();
    if (m_nodes.size() > 1)
    {
        ImNodes::MiniMap();
    }
    ImNodes::EndNodeEditor();
    handleDelete();
    handleConnect();
}

void NodeEditor::to_json(nlohmann::json& j) const
{
    using nlohmann::json;

    auto node_array = json::array();
    NodeSerializer serializer(m_midi_engine);
    for (const auto& node : m_nodes)
    {
        json node_json;
        serializer.serialize_node(node_json, *node);
        node_array.push_back(node_json);
    }
    const ImVec2 panning = ImNodes::EditorContextGetPanning();
    j = json{
        { "nodes", node_array },
        { "panning", panning }
    };
}

std::unique_ptr<NodeEditor> NodeEditor::from_json(midi::Engine& midi_engine, const nlohmann::json& j)
{
    const ImVec2 panning = j["panning"];
    ImNodes::EditorContextResetPanning(panning);

    auto editor = std::make_unique<NodeEditor>(midi_engine);
    NodeSerializer deserializer(midi_engine);

    // 1st iter -> create nodes
    std::map<int, int> old_to_new_node_id;
    for (const auto& node_json : j["nodes"])
    {
        auto& node = *editor->m_nodes.emplace_back(deserializer.deserialize_node(node_json));
        old_to_new_node_id[node_json["id"]] = node.id();
    }

    // 2nd iter -> create connections
    for (const auto& node_json : j["nodes"])
    {
        const int node_id = old_to_new_node_id[node_json["id"]];
        const auto source_node = *std::find_if(editor->m_nodes.begin(), editor->m_nodes.end(),
            [node_id](const auto& node) { return node->id() == node_id; });

        for (const int old_connected_node_id : node_json["output_connection_ids"])
        {
            const int connected_node_id = old_to_new_node_id[old_connected_node_id];
            const auto target_node = *std::find_if(editor->m_nodes.begin(), editor->m_nodes.end(),
                [connected_node_id](const auto& node) { return node->id() == connected_node_id; });
            source_node->connect_output(std::weak_ptr(target_node), std::weak_ptr(source_node));
        }
    }

    return editor;
}

void NodeEditor::renderContextMenu()
{
    constexpr ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    auto render_contents = [this, leaf_flags](auto& infos)
    {
        for (const auto& info : infos)
        {
            ImGui::TreeNodeEx(info.m_name.c_str(), leaf_flags);
            if (ImGui::IsItemClicked())
            {
                using node_type = std::conditional_t<std::is_same_v<midi::InputInfo, std::decay_t<decltype(info)>>,
                    MidiInNode, MidiOutNode>;
                
                const auto& node = m_nodes.emplace_back(std::make_shared<node_type>(info, m_midi_engine));

                ImNodes::SetNodeScreenSpacePos(node->id(), ImGui::GetMousePosOnOpeningCurrentPopup());
                ImGui::CloseCurrentPopup();
            }
        }
    };

    ImGui::SetNextWindowSizeConstraints({300, 0}, {500, 500});
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::IsWindowAppearing())
        {
            m_input_infos = midi::Probe::get_inputs();
            m_output_infos = midi::Probe::get_outputs();
        }
        ImGui::TreeNodeEx("Channel map", leaf_flags);
        if (ImGui::IsItemClicked())
        {
            const auto& node = m_nodes.emplace_back(std::make_shared<MidiChannelNode>());
            ImNodes::SetNodeScreenSpacePos(node->id(), ImGui::GetMousePosOnOpeningCurrentPopup());
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::TreeNode("MIDI inputs"))
        {
            render_contents(m_input_infos);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("MIDI outputs"))
        {
            render_contents(m_output_infos);
            ImGui::TreePop();
        }
        ImGui::EndPopup();
    }
}

void NodeEditor::renderNodes()
{
    for (const auto& node : m_nodes)
    {
        node->render();
    }
}

void NodeEditor::handleDelete()
{
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))
        || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
    {
        std::vector<int> selected_ids;
        selected_ids.resize(ImNodes::NumSelectedNodes());
        if (!selected_ids.empty())
        {
            ImNodes::GetSelectedNodes(selected_ids.data());
            // get nodes to remove
            m_nodes.erase(
                std::remove_if(
                    m_nodes.begin(),
                    m_nodes.end(),
                    [&selected_ids](const auto& node)
                    {
                        return selected_ids.cend() !=
                            std::find(selected_ids.cbegin(), selected_ids.cend(), node->id());
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
        auto start_node_it = std::find_if(m_nodes.begin(), m_nodes.end(),
            [start_attrib_id](const auto& node) { return node->out_id() == start_attrib_id; });
        auto end_node_it = std::find_if(m_nodes.begin(), m_nodes.end(),
            [end_attrib_id](const auto& node) { return node->in_id() == end_attrib_id; });
        (*start_node_it)->connect_output(std::weak_ptr(*end_node_it), std::weak_ptr(*start_node_it));
    }
}

}
