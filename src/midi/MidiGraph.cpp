#include "MidiGraph.hpp"

namespace mc::midi
{

Node::~Node()
{
    for (auto& conn : m_sender_connections)
    {
        Connection::destroy(conn);
    }
    for (auto& conn : m_receiver_connections)
    {
        Connection::destroy(conn);
    }
}

void Node::process(data_span data)
{
}

void Node::broadcast_data(data_span data)
{
    size_t idx = 0;
    for (auto it = m_sender_connections.begin(), end = m_sender_connections.end(); it != end;
         ++it, ++idx)
    {
        if (idx == 0)
        {
            (*it)->receiver()->receive_data(data);
        }
        else
        {
            data_vec data_copy(data.size());
            std::copy(data.begin(), data.end(), data_copy.begin());
            (*it)->receiver()->receive_data(data_copy);
        }
    }
}

void Node::connect_as_sender(connection_ptr connection)
{
    m_sender_connections.emplace(connection);
}

void Node::connect_as_receiver(connection_ptr connection)
{
    m_receiver_connections.emplace(connection);
}

void Node::remove_connection(connection_ptr connection)
{
    remove_connection_impl(m_sender_connections, connection);
    remove_connection_impl(m_receiver_connections, connection);
}

void Node::remove_connection_impl(std::set<connection_ptr>& set, connection_ptr connection)
{
    set.erase(connection);
}

void Node::receive_data(data_span data)
{
    process(data);
    broadcast_data(data);
}

weak_connection_ptr Connection::create(Node& sender, Node& receiver)
{
    if (is_loop(sender, receiver))
    {
        return weak_connection_ptr();
    }
    const auto conn  = std::shared_ptr<Connection>(new Connection());
    conn->m_sender   = &sender;
    conn->m_receiver = &receiver;
    sender.connect_as_sender(conn);
    receiver.connect_as_receiver(conn);
    return conn;
}

void Connection::destroy(weak_connection_ptr connection)
{
    auto connection_shared = connection.lock();
    if (connection_shared)
    {
        connection_shared->m_sender->remove_connection(connection_shared);
        connection_shared->m_receiver->remove_connection(connection_shared);
    }
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
