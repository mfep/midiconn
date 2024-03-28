#pragma once
#include <cstdlib>
#include <map>
#include <memory>
#include <shared_mutex>

#include "InputNode.hpp"
#include "MidiInfo.hpp"
#include "OutputNode.hpp"

namespace mc::midi
{

class Engine final
{
public:
    void create(const InputInfo& input_info, InputObserver* observer = nullptr);
    void create(const OutputInfo& output_info, OutputObserver* observer = nullptr);
    void remove(const InputInfo& input_info, InputObserver* observer = nullptr);
    void remove(const OutputInfo& output_info, OutputObserver* observer = nullptr);
    void connect(size_t input_id, size_t output_id, channel_map channels);
    void disconnect(size_t input_id, size_t output_id);
    void enable_message_types(const MessageTypeMask& mask);

    const MessageTypeMask& get_message_types() const { return m_message_type_mask; }

private:
    std::map<size_t, InputNode>                              m_inputs;
    std::map<size_t, OutputNode>                             m_outputs;
    std::map<std::pair<size_t, size_t>, weak_connection_ptr> m_connections;
    std::shared_mutex                                        m_mutex;
    MessageTypeMask                                          m_message_type_mask;
};

} // namespace mc::midi
