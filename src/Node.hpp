#pragma once
#include <map>
#include <memory>
#include <vector>

#include "nlohmann/json_fwd.hpp"

#include "Time.hpp"
#include "midi/GraphObserver.hpp"
#include "midi/MidiInfo.hpp"

namespace mc
{
namespace midi
{
class Node;
}

class NodeSerializer;
class ScaleProvider;

class Node : private midi::GraphObserver
{
public:
    using node_ptr = std::weak_ptr<Node>;
    using conn     = std::pair<node_ptr, int>;

    Node(const ScaleProvider& scale_provider, midi::Node* midi_node);
    virtual ~Node();
    void render();
    void connect_output(node_ptr to_node, node_ptr this_node);
    void disconnect_output(int link_id);

    int                   id() const { return m_id; }
    int                   in_id() const { return 2 * m_id; }
    int                   out_id() const { return 2 * m_id + 1; }
    std::vector<node_ptr> get_output_connections() const;
    std::vector<node_ptr> get_input_connections() const;

    virtual void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const = 0;

    static bool is_in_id(int id) { return id % 2 == 0; }
    static bool is_out_id(int id) { return !is_in_id(id); }

    virtual midi::Node* get_midi_node() const { return m_midi_node; }
    virtual void        render_inspector() {}

protected:
    virtual void render_internal() = 0;
    virtual void push_style() const {}
    virtual void pop_style() const {}

    void message_processed(std::span<const unsigned char> message_bytes) override;
    void message_received(std::span<const unsigned char> message_bytes) override;

    void begin_input_attribute() const;
    void end_input_attribute() const;
    void begin_output_attribute() const;
    void end_output_attribute() const;

    const ScaleProvider* m_scale_provider;

private:
    void            connect_input(node_ptr from_node, int link_id);
    void            disconnect_input(int link_id);
    static uint32_t get_pin_color(time_point_t last_activity);

    static inline int sm_next_id{};
    static inline int sm_next_link_id{};

    // self generated node id
    int m_id;

    // maps self generated link id to node
    std::map<int, node_ptr> m_output_connections;

    // maps link id received from connecting node to node
    std::map<int, node_ptr> m_input_connections;

    midi::Node* m_midi_node;

    time_point_t m_last_input_activity{};
    time_point_t m_last_output_activity{};

    friend class NodeSerializer;
};

} // namespace mc
