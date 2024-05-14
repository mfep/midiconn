#include "MidiGraph.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cassert>

namespace mc::midi
{

Node::~Node()
{
    for (auto& conn : m_sender_connections)
    {
        [[maybe_unused]] const bool removed = conn->m_receiver->remove_receiver_connection(conn);
        assert(removed);
    }
    for (auto& conn : m_receiver_connections)
    {
        [[maybe_unused]] const bool removed = conn->m_sender->remove_sender_connection(conn);
        assert(removed);
    }
    m_sender_connections.clear();
    m_receiver_connections.clear();
}

bool Node::process(data_span /*data*/)
{
    return true;
}

void Node::broadcast_data(data_span data)
{
    for (auto it = m_sender_connections.begin(), end = m_sender_connections.end(); it != end; ++it)
    {
        data_vec data_copy(data.begin(), data.end());
        (*it)->receiver()->receive_data(data_copy);
    }
}

bool Node::connect_as_sender(connection_ptr connection)
{
    auto [it, emplaced] = m_sender_connections.emplace(connection);
    return emplaced;
}

bool Node::connect_as_receiver(connection_ptr connection)
{
    auto [it, emplaced] = m_receiver_connections.emplace(connection);
    return emplaced;
}

bool Node::remove_sender_connection(connection_ptr connection)
{
    return m_sender_connections.erase(connection) > 0;
}

bool Node::remove_receiver_connection(connection_ptr connection)
{
    return m_receiver_connections.erase(connection) > 0;
}

void Node::receive_data(data_span data)
{
    raise_message_received(data);
    if (process(data))
    {
        raise_message_processed(data);
        broadcast_data(data);
    }
}

Connection::~Connection()
{
    spdlog::info("Disconnected sender \"{}\" from receiver \"{}\"", m_sender_name, m_receiver_name);
}

bool Connection::create(Node& sender, Node& receiver)
{
    if (is_loop(sender, receiver))
    {
        return false;
    }
    const auto conn       = std::shared_ptr<Connection>(new Connection());
    conn->m_sender        = &sender;
    conn->m_receiver      = &receiver;
    conn->m_sender_name   = sender.name();
    conn->m_receiver_name = receiver.name();
    sender.connect_as_sender(conn);
    receiver.connect_as_receiver(conn);
    spdlog::info("Connected sender \"{}\" to receiver \"{}\"", sender.name(), receiver.name());
    return true;
}

bool Connection::disconnect(Node& sender, Node& receiver)
{
    connection_ptr found_common_connection;
    for (auto sender_it     = sender.m_sender_connections.begin(),
              sender_end_it = sender.m_sender_connections.end();
         found_common_connection == nullptr && sender_it != sender_end_it;
         ++sender_it)
    {
        for (auto receiver_it     = receiver.m_receiver_connections.begin(),
                  receiver_end_it = receiver.m_receiver_connections.end();
             found_common_connection == nullptr && receiver_it != receiver_end_it;
             ++receiver_it)
        {
            if (*sender_it == *receiver_it)
            {
                found_common_connection = *sender_it;
            }
        }
    }
    if (found_common_connection)
    {
        found_common_connection->m_receiver->remove_receiver_connection(found_common_connection);
        found_common_connection->m_sender->remove_sender_connection(found_common_connection);
    }
    return found_common_connection != nullptr;
}

bool Connection::is_loop(Node& sender, Node& receiver)
{
    return &sender == &receiver || std::any_of(receiver.m_sender_connections.begin(),
                                               receiver.m_sender_connections.end(),
                                               [&sender](const connection_ptr& connection) {
                                                   return is_loop(sender, *connection->m_receiver);
                                               });
}

} // namespace mc::midi
