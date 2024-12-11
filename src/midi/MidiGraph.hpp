#pragma once

#include "GraphObserver.hpp"

#include <memory>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace mc::midi
{

class Connection;

using connection_ptr = std::shared_ptr<Connection>;
using data_vec       = std::vector<unsigned char>;
using data_span      = std::span<unsigned char>;

class Node : public GraphObservable
{
public:
    virtual ~Node();

protected:
    virtual std::string_view name() = 0;
    virtual bool             process(data_span data);
    void                     broadcast_data(data_span data);

private:
    friend class Connection;

    bool connect_as_sender(connection_ptr connection);
    bool connect_as_receiver(connection_ptr connection);
    bool remove_sender_connection(connection_ptr connection);
    bool remove_receiver_connection(connection_ptr connection);

    void receive_data(data_span data);

    std::set<connection_ptr> m_sender_connections;
    std::set<connection_ptr> m_receiver_connections;
};

class Connection final
{
private:
    Connection() = default;

public:
    ~Connection();

    static bool create(Node& sender, Node& receiver);
    static bool disconnect(Node& sender, Node& receiver);

private:
    friend class Node;

    Node*       receiver() const { return m_receiver; }
    static bool is_loop(Node& sender, Node& receiver);

    Node* m_sender;
    Node* m_receiver;

    std::string m_sender_name;
    std::string m_receiver_name;
};

} // namespace mc::midi
