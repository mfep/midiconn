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
        for (const auto& item : infos)
        {
            if (ImGui::Button(item.m_name.c_str()))
            {
                const auto node_id = m_current_node_id++;
                m_nodes.push_back({ item, node_id });
                imnodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePosOnOpeningCurrentPopup());
                ImGui::CloseCurrentPopup();
                raise_node_created(item);
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
        switch (node.m_info.index())
        {
            case 0:
                imnodes::BeginOutputAttribute(node.m_id);
                ImGui::TextUnformatted("MIDI input (all channels)");
                imnodes::EndOutputAttribute();
                break;
            case 1:
                imnodes::BeginInputAttribute(node.m_id);
                ImGui::TextUnformatted("MIDI output (all channels)");
                imnodes::EndInputAttribute();
                break;
            default:
                throw std::logic_error("Unexpected case");
        }
        imnodes::EndNode();
    }

    for (const auto& link : m_links)
    {
        imnodes::Link(link.m_id, link.m_start_id, link.m_end_id);
    }
}

void NodeEditor::handleDelete()
{
    auto remove_links = [this](auto remove_links_start)
    {
        // signal about removing links
        std::for_each(remove_links_start, m_links.end(), [this](const auto& link)
        {
            auto start_node_it = std::find_if(m_nodes.begin(), m_nodes.end(),
                [id=link.m_start_id](const auto& node) { return node.m_id == id; });
            auto end_node_it = std::find_if(m_nodes.begin(), m_nodes.end(),
                [id=link.m_end_id](const auto& node) { return node.m_id == id; });
            raise_link_destroyed(
                std::get<midi::InputInfo>(start_node_it->m_info),
                std::get<midi::OutputInfo>(end_node_it->m_info));
        });
        // erase links
        m_links.erase(remove_links_start, m_links.end());
    };

    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))
        || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
    {
        std::vector<int> selected_ids;
        selected_ids.resize(imnodes::NumSelectedNodes());
        if (!selected_ids.empty())
        {
            imnodes::GetSelectedNodes(selected_ids.data());
            // get nodes to remove
            auto remove_nodes_start = std::partition(
                m_nodes.begin(),
                m_nodes.end(),
                [&selected_ids](const auto& node)
                {
                    return selected_ids.cend() ==
                        std::find(selected_ids.cbegin(), selected_ids.cend(), node.m_id);
                });

            // get connected links to remove
            auto remove_links_start = std::partition(
                m_links.begin(),
                m_links.end(),
                [this, remove_nodes_start](const auto& link)
                {
                    return m_nodes.end() == std::find_if(
                        remove_nodes_start,
                        m_nodes.end(),
                        [&link](const auto& removed_node)
                        {
                            return link.m_start_id == removed_node.m_id || link.m_end_id == removed_node.m_id;
                        });
                });

            remove_links(remove_links_start);
            // signal about removing nodes
            std::for_each(remove_nodes_start, m_nodes.end(), [this](const auto& node)
            {
                std::visit([this](const auto& info) { raise_node_destroyed(info); }, node.m_info);
            });
            // remove nodes
            m_nodes.erase(remove_nodes_start, m_nodes.end());
        }

        selected_ids.clear();
        selected_ids.resize(imnodes::NumSelectedLinks());
        if (!selected_ids.empty())
        {
            imnodes::GetSelectedLinks(selected_ids.data());
            auto remove_links_start = std::partition(
                m_links.begin(),
                m_links.end(),
                [&selected_ids](const auto& link)
                {
                    return selected_ids.cend() == std::find(
                        selected_ids.cbegin(),
                        selected_ids.cend(),
                        link.m_id);
                });
            remove_links(remove_links_start);
        }
    }
}

void NodeEditor::handleConnect()
{
    int start_attrib_id;
    int end_attrib_id;
    if (imnodes::IsLinkCreated(&start_attrib_id, &end_attrib_id))
    {
        m_links.push_back({ m_current_link_id++, start_attrib_id, end_attrib_id });
        auto start_node_it = std::find_if(m_nodes.begin(), m_nodes.end(),
            [start_attrib_id](const auto& node) { return node.m_id == start_attrib_id; });
        auto end_node_it = std::find_if(m_nodes.begin(), m_nodes.end(),
            [end_attrib_id](const auto& node) { return node.m_id == end_attrib_id; });
        raise_link_created(
            std::get<midi::InputInfo>(start_node_it->m_info),
            std::get<midi::OutputInfo>(end_node_it->m_info));
    }
}

}
