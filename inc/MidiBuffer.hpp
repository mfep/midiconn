#pragma once
#include <cassert>
#include <vector>

namespace mc::midi
{

constexpr size_t midi_message_bytes = 3;

class MessageView final
{
public:
    MessageView(unsigned char* message_ptr) :
        m_message_ptr(message_ptr)
    {
    }

    inline MessageView& operator++()
    {
        m_message_ptr += midi_message_bytes;
        return *this;
    }

    inline bool operator!=(const MessageView& other)
    {
        return m_message_ptr != other.m_message_ptr;
    }

    inline bool is_system() const
    {
        // system messages start with 0b1111xxxx
        return (m_message_ptr[0] & 0xf0) == 0xf0;
    }

    inline char get_channel() const
    {
        // channel is cccc in 0bxxxxcccc of the first byte
        // if not a system message
        assert(!is_system());
        return m_message_ptr[0] & 0x0f;
    }

    inline void set_channel(char ch)
    {
        // channel is cccc in 0bxxxxcccc of the first byte
        // if not a system message
        assert(!is_system());
        m_message_ptr[0] = (m_message_ptr[0] & 0xf0) | (ch & 0x0f);
    }

private:
    unsigned char* m_message_ptr;
};

class MidiBuffer final
{
public:
    MidiBuffer(std::vector<unsigned char>& buffer) :
        m_buffer(buffer)
    {
        // ToDo check: does not seem to always hold
        assert(buffer.size() % midi_message_bytes == 0);
    }

    MessageView begin()
    {
        return MessageView(m_buffer.data());
    }

    MessageView end()
    {
        return MessageView(m_buffer.data() + m_buffer.size());
    }

private:
    std::vector<unsigned char>& m_buffer;
};

}
