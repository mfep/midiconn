#pragma once

#include "ActivityIndicator.hpp"
#include "Node.hpp"
#include "ScaleProvider.hpp"
#include "midi/MidiGraph.hpp"

#include <array>
#include <chrono>
#include <cstdint>
#include <deque>
#include <mutex>
#include <span>
#include <string_view>

namespace mc
{

class LogNode final : public Node, private midi::GraphObserver
{
private:
    class LogMidiNode final : public midi::Node
    {
    public:
        std::string_view name() override;
    };

public:
    explicit LogNode(const ScaleProvider& scale_provider);
    ~LogNode();

    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const override;

protected:
    midi::Node* get_midi_node() override;
    void        render_internal() override;

private:
    static inline constexpr std::size_t default_max_buffer_size = 500;

    void message_received(std::span<const unsigned char> message_bytes) override;

    struct BufferElement
    {
        BufferElement(std::string&&        name,
                      std::optional<int>&& channel,
                      std::string&&        data_0,
                      std::string&&        data_1)
            : m_arrived(std::chrono::system_clock::now()), m_name(std::move(name)),
              m_channel(std::move(channel)), m_data_0(std::move(data_0)),
              m_data_1(std::move(data_1))
        {
        }

        std::chrono::time_point<std::chrono::system_clock> m_arrived;
        std::string                                        m_name;
        std::optional<int>                                 m_channel;
        std::string                                        m_data_0;
        std::string                                        m_data_1;
    };

    std::mutex                m_buffer_mutex;
    std::deque<BufferElement> m_message_buffer;
    LogMidiNode               m_log_midi_node;
    ActivityIndicator         m_input_indicator;
    std::size_t               m_max_buffer_size = default_max_buffer_size;
    const ScaleProvider*      m_scale_provider;

    friend class NodeSerializer;
};

} // namespace mc
