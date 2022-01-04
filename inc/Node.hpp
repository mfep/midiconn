#pragma once
#include <array>
#include <map>
#include <memory>
#include <vector>

namespace mc
{

class Node
{
public:
    using node_ptr = std::weak_ptr<Node>;
    using conn = std::pair<node_ptr, int>;
    using channel_map = std::array<char, 16>;
    using midi_sources = std::map<size_t, channel_map>;

    Node();
    virtual ~Node() = default;
    void render();
    void connect_output(node_ptr to_node, node_ptr this_node);
    void disconnect_output(int link_id);

    int id() const { return m_id; }
    int in_id() const { return 2 * m_id; }
    int out_id() const { return 2 * m_id + 1; }

protected:
    virtual void render_internal() = 0;
    virtual channel_map transform_channel_map(const channel_map& in_map);
    virtual void update_outputs();

    midi_sources m_sources;

private:
    const midi_sources& get_sources() const { return m_sources; }
    void connect_input(node_ptr from_node, int link_id);
    void disconnect_input(int link_id);
    void update_inputs(int new_link_id = -1);

    static inline int sm_next_id{};
    static inline int sm_next_link_id{};
    
    int m_id;
    std::map<int, node_ptr> m_output_connections;
    std::map<int, node_ptr> m_input_connections;
};

}
