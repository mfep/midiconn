#pragma once

#include <algorithm>
#include <memory>
#include <set>
#include <span>
#include <vector>

namespace mc::midi
{

class Connection;

using connection_ptr      = std::shared_ptr<Connection>;
using weak_connection_ptr = std::weak_ptr<Connection>;
using data_vec            = std::vector<unsigned char>;
using data_span           = std::span<unsigned char>;

class Node
{
public:
    virtual ~Node();

protected:
    virtual void process(data_span data);
    void         broadcast_data(data_span data);

private:
    friend class Connection;

    void connect_as_sender(connection_ptr connection);
    void connect_as_receiver(connection_ptr connection);
    void remove_connection(connection_ptr connection);
    void remove_connection_impl(std::set<connection_ptr>& set, connection_ptr connection);

    void receive_data(data_span data);

    std::set<connection_ptr> m_sender_connections;
    std::set<connection_ptr> m_receiver_connections;
};

class Connection final
{
private:
    Connection() = default;

public:
    static weak_connection_ptr create(Node& sender, Node& receiver);
    static void                destroy(weak_connection_ptr connection);

    Node* receiver() const { return m_receiver; }

private:
    static bool is_loop(Node& sender, Node& receiver);

    Node* m_sender;
    Node* m_receiver;
};

} // namespace mc::midi
