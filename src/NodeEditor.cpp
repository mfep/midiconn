#include "NodeEditor.hpp"

#include <algorithm>

#include "imgui.h"
#include "imnodes.h"

namespace mc::display
{

void NodeEditor::render()
{
    imnodes::BeginNodeEditor();
    renderContextMenu();
    renderNodes();
    imnodes::EndNodeEditor();
    handleDelete();
    handleConnect();
}

void NodeEditor::renderContextMenu()
{
    auto render_contents = [this](auto& infos)
    {
        for (const auto& info : infos)
        {
            constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            ImGui::TreeNodeEx(info.m_name.c_str(), flags);
            if (ImGui::IsItemClicked())
            {
                const auto& node = m_nodes.emplace_back(std::make_shared<TestNode>());
                imnodes::SetNodeScreenSpacePos(node->id(), ImGui::GetMousePosOnOpeningCurrentPopup());
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
        selected_ids.resize(imnodes::NumSelectedNodes());
        if (!selected_ids.empty())
        {
            imnodes::GetSelectedNodes(selected_ids.data());
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
        selected_ids.resize(imnodes::NumSelectedLinks());
        if (!selected_ids.empty())
        {
            imnodes::GetSelectedLinks(selected_ids.data());
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
    if (imnodes::IsLinkCreated(&start_attrib_id, &end_attrib_id))
    {
        auto start_node_it = std::find_if(m_nodes.begin(), m_nodes.end(),
            [start_attrib_id](const auto& node) { return node->out_id() == start_attrib_id; });
        auto end_node_it = std::find_if(m_nodes.begin(), m_nodes.end(),
            [end_attrib_id](const auto& node) { return node->in_id() == end_attrib_id; });
        (*start_node_it)->connect_output(std::weak_ptr(*end_node_it));
    }
}

}
