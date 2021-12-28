#include "NodeEditor.hpp"

#include <algorithm>

#include "imgui.h"
#include "imnodes.h"

#include "MidiEngine.hpp"

namespace mc::display
{

const std::string& NodeEditor::Node::get_name() const
{
    return std::visit(
        [](const auto& var) -> const std::string& { return var.m_name; },
        m_info);
}

const char* NodeEditor::Node::get_attribute_caption() const
{
    switch (m_info.index())
    {
        case 0: return "MIDI input (all channels)";
        case 1: return "MIDI output (all channels)";
        default: return nullptr;
    }
}

void NodeEditor::render()
{
    imnodes::BeginNodeEditor();
    renderContextMenu();
    renderNodes();
    imnodes::EndNodeEditor();
    handleDelete();
}

void NodeEditor::renderContextMenu()
{
    auto render_contents = [this](auto& infos)
    {
        for (const auto& item : infos)
        {
            if (ImGui::Button(item.m_name.c_str()))
            {
                const auto node_id = m_current_node_id++;
                m_nodes.push_back({ item, node_id });
                imnodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePosOnOpeningCurrentPopup());
                ImGui::CloseCurrentPopup();
            }
        }
    };

    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::IsWindowAppearing())
        {
            m_input_infos = midi::Probe::get_inputs();
            m_output_infos = midi::Probe::get_outputs();
        }
        if (ImGui::CollapsingHeader("MIDI inputs"))
        {
            render_contents(m_input_infos);
        }
        if (ImGui::CollapsingHeader("MIDI outputs"))
        {
            render_contents(m_output_infos);
        }
        ImGui::EndPopup();
    }
}

void NodeEditor::renderNodes()
{
    for (const auto& node : m_nodes)
    {
        imnodes::BeginNode(node.m_id);
        imnodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(node.get_name().c_str());
        imnodes::EndNodeTitleBar();
        imnodes::BeginOutputAttribute(node.m_id);
        ImGui::TextUnformatted(node.get_attribute_caption());
        imnodes::EndOutputAttribute();
        imnodes::EndNode();
    }
}

void NodeEditor::handleDelete()
{
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))
        || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
    {
        std::vector<int> selected_node_ids;
        selected_node_ids.resize(imnodes::NumSelectedNodes());
        if (!selected_node_ids.empty())
        {
            imnodes::GetSelectedNodes(selected_node_ids.data());
            m_nodes.erase(
                std::remove_if(
                    m_nodes.begin(),
                    m_nodes.end(),
                    [&selected_node_ids](const auto& node)
                    {
                        return selected_node_ids.cend() !=
                            std::find(selected_node_ids.cbegin(), selected_node_ids.cend(), node.m_id);
                    }),
                m_nodes.end());
        }
    }
}

}
