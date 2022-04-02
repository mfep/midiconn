#pragma once
#include <map>
#include <memory>
#include <vector>
#include <nlohmann/json_fwd.hpp>
#include "MidiInfo.hpp"

namespace mc
{

class NodeSerializer;

class Node
{
public:
    using node_ptr = std::weak_ptr<Node>;
    using conn = std::pair<node_ptr, int>;
    using midi_sources = std::map<size_t, midi::channel_map>;

    Node();
    virtual ~Node() = default;
    void render();
    void connect_output(node_ptr to_node, node_ptr this_node);
    void disconnect_output(int link_id);

    int id() const { return m_id; }
    int in_id() const { return 2 * m_id; }
    int out_id() const { return 2 * m_id + 1; }

    virtual void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const = 0;

protected:
    virtual void render_internal() = 0;
    virtual midi::channel_map transform_channel_map(const midi::channel_map& in_map);
    virtual void update_outputs_with_sources();

    // sources read from inputs
    midi_sources m_input_sources;

private:
    midi_sources get_transformed_sources();
    void connect_input(node_ptr from_node, int link_id);
    void disconnect_input(int link_id);
    void update_sources_from_inputs(int new_link_id = -1);

    static inline int sm_next_id{};
    static inline int sm_next_link_id{};

    // self generated node id
    int m_id;

    // maps self generated link id to node
    std::map<int, node_ptr> m_output_connections;

    // maps link id received from connecting node to node
    std::map<int, node_ptr> m_input_connections;

    friend class NodeSerializer;
};

}
