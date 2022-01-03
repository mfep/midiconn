#pragma once
#include <memory>
#include <vector>

namespace mc
{

class Node
{
public:
    using data_vec = std::vector<unsigned char>;
    using data_itr = typename data_vec::iterator;
    using const_data_itr = typename data_vec::const_iterator;
    using node_ptr = std::weak_ptr<Node>;
    using out_conn = std::pair<node_ptr, int>;

    Node();
    virtual ~Node() = default;
    void render();
    void process(size_t length, const_data_itr in_itr);
    void connect_output(node_ptr other_node);
    void disconnect_output(int link_id);
    int id() const { return m_id; }
    int in_id() const { return 2 * m_id; }
    int out_id() const { return 2 * m_id + 1; }

protected:
    virtual void render_internal() = 0;
    virtual void process_internal(size_t size, const_data_itr in_itr, data_itr out_itr) = 0;

private:
    static inline int sm_next_id{};
    static inline int sm_next_link_id{};
    int m_id;
    std::vector<out_conn> m_output_connections;
    data_vec m_buffer;
};

class TestNode final : public Node
{
private:
    void render_internal() override;
    void process_internal(size_t size, const_data_itr in_itr, data_itr out_itr) override;
};

}
