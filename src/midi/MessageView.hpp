#pragma once
#include <cassert>
#include <span>

namespace mc::midi
{

class MessageView final
{
public:
    explicit MessageView(std::span<unsigned char> message_data) : m_message_data(message_data)
    {
        assert(!m_message_data.empty());
    }

    bool is_system() const
    {
        // system messages start with 0b1111xxxx
        return (m_message_data[0] & 0xf0) == 0xf0;
    }

    char get_channel() const
    {
        // channel is cccc in 0bxxxxcccc of the first byte
        // if not a system message
        assert(!is_system());
        return m_message_data[0] & 0x0f;
    }

    void set_channel(unsigned char ch)
    {
        // channel is cccc in 0bxxxxcccc of the first byte
        // if not a system message
        assert(!is_system());
        m_message_data[0] = (m_message_data[0] & 0xf0) | (ch & 0x0f);
    }

private:
    std::span<unsigned char> m_message_data;
};

} // namespace mc::midi
