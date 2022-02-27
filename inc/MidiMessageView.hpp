#pragma once
#include <cassert>
#include <vector>

namespace mc::midi
{

class MessageView final
{
public:
    MessageView(std::vector<unsigned char>& message_data) :
        m_message_data(message_data)
    {
        assert(!m_message_data.empty());
    }

    inline bool is_system() const
    {
        // system messages start with 0b1111xxxx
        return (m_message_data[0] & 0xf0) == 0xf0;
    }

    inline char get_channel() const
    {
        // channel is cccc in 0bxxxxcccc of the first byte
        // if not a system message
        assert(!is_system());
        return m_message_data[0] & 0x0f;
    }

    inline void set_channel(char ch)
    {
        // channel is cccc in 0bxxxxcccc of the first byte
        // if not a system message
        assert(!is_system());
        m_message_data[0] = (m_message_data[0] & 0xf0) | (ch & 0x0f);
    }

private:
    std::vector<unsigned char>& m_message_data;
};

}
