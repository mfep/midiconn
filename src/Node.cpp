#include "Node.hpp"
#include <algorithm>
#include "imnodes.h"

namespace mc
{

Node::Node() :
    m_id(sm_next_id++)
{
}

void Node::render()
{
    imnodes::BeginNode(m_id);
    render_internal();
    imnodes::EndNode();

    m_output_connections.erase(
        std::remove_if(
            m_output_connections.begin(),
            m_output_connections.end(),
            [](const auto& out) { return std::get<0>(out).expired(); }),
        m_output_connections.end());

    for (const auto&[node, link_id] : m_output_connections)
    {
        imnodes::Link(link_id, m_id, std::shared_ptr(node)->m_id);
    }
}

void Node::process(size_t length, const_data_itr in_itr)
{
    if (length > m_buffer.size())
    {
        m_buffer.resize(length);
    }
    process_internal(length, in_itr, m_buffer.begin());

    for (const auto& conn : m_output_connections)
    {
        auto other_node = std::get<0>(conn).lock();
        if (other_node != nullptr)
        {
            other_node->process(length, m_buffer.begin());
        }
    }
}

void Node::connect_output(node_ptr other_node)
{
    auto found_connection = std::find_if(
        m_output_connections.cbegin(),
        m_output_connections.cend(),
        [other_node](const auto& connection) { return std::get<0>(connection).lock() == other_node.lock(); });

    if (found_connection == m_output_connections.cend())
    {
        m_output_connections.emplace_back(other_node, sm_next_link_id++);
    }
}

void Node::disconnect_output(node_ptr other_node)
{
    auto found_connection = std::find_if(
        m_output_connections.cbegin(),
        m_output_connections.cend(),
        [other_node](const auto& connection) { return std::get<0>(connection).lock() == other_node.lock(); });

    if (found_connection != m_output_connections.end())
    {
        m_output_connections.erase(found_connection);
    }
}

}
