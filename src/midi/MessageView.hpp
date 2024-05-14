#pragma once

#include "../Utils.hpp"

#include <cassert>
#include <span>
#include <string_view>
#include <type_traits>
#include <variant>

namespace mc::midi
{

template <class T>
using message_view_tag_t = typename T::tag_t;

template <class... Funs>
struct tag_overloads : Funs...
{
    template <class MessageV>
    decltype(auto) operator()(MessageV&& message_view)
    {
        return this->operator()(message_view_tag_t<MessageV>{},
                                std::forward<MessageV>(message_view));
    }

private:
    using Funs::operator()...;
};

template <class... Funs>
tag_overloads(Funs...) -> tag_overloads<Funs...>;

struct MessageViewTag
{
};

template <bool Mutable>
class ChannelMessageView;
template <bool Mutable>
class NoteOffMessageView;
template <bool Mutable>
class NoteOnMessageView;
template <bool Mutable>
class PolyKeyPressureMessageView;
template <bool Mutable>
class ControlChangeMessageView;
template <bool Mutable>
class ChannelModeMessageView;
template <bool Mutable>
class ProgramChangeMessageView;
template <bool Mutable>
class ChannelPressureMessageView;
template <bool Mutable>
class PitchBendMessageView;
class AllSoundOffMessageView;
class ResetAllControllersMessageView;
template <bool Mutable>
class LocalControlMessageView;
class AllNotesOffMessageView;
class OmniModeOffMessageView;
class OmniModeOnMessageView;
template <bool Mutable>
class MonoModeOnMessageView;
class PolyModeOnMessageView;

template <bool Mutable = true>
class MessageView
{
public:
    using tag_t = MessageViewTag;
    using span_t =
        std::conditional_t<Mutable, std::span<unsigned char>, std::span<const unsigned char>>;

    explicit MessageView(span_t message_data) : m_message_data(message_data)
    {
        assert(!m_message_data.empty());
    }

    span_t get_bytes() { return m_message_data; }

    bool is_system() const
    {
        // system messages start with 0b1111xxxx
        return (m_message_data[0] & 0xf0) == 0xf0;
    }

    bool is_channel() const { return !is_system(); }

    std::variant<NoteOnMessageView<Mutable>,
                 NoteOffMessageView<Mutable>,
                 PolyKeyPressureMessageView<Mutable>,
                 ControlChangeMessageView<Mutable>,
                 ChannelModeMessageView<Mutable>,
                 ProgramChangeMessageView<Mutable>,
                 ChannelPressureMessageView<Mutable>,
                 PitchBendMessageView<Mutable>,
                 AllSoundOffMessageView,
                 ResetAllControllersMessageView,
                 LocalControlMessageView<Mutable>,
                 AllNotesOffMessageView,
                 OmniModeOffMessageView,
                 OmniModeOnMessageView,
                 MonoModeOnMessageView<Mutable>,
                 PolyModeOnMessageView,
                 ChannelMessageView<Mutable>,
                 MessageView<Mutable>>
    parse() const
    {
        if (is_channel())
        {
            return utils::variant_cast(ChannelMessageView<Mutable>(m_message_data).parse());
        }
        return *this;
    }

protected:
    span_t m_message_data;
};

MessageView(std::span<unsigned char>) -> MessageView<true>;
MessageView(std::span<const unsigned char>) -> MessageView<false>;

struct ChannelMessageViewTag
{
};

template <bool Mutable = true>
class ChannelMessageView : public MessageView<Mutable>
{
public:
    using tag_t = ChannelMessageViewTag;
    explicit ChannelMessageView(MessageView<Mutable>::span_t message_data)
        : MessageView<Mutable>(message_data)
    {
    }

    char get_channel() const
    {
        // channel is cccc in 0bxxxxcccc of the first byte
        return this->m_message_data[0] & 0x0f;
    }

    char get_channel_human() const { return get_channel() + 1; }

    template <bool M = Mutable>
    auto set_channel(unsigned char ch) -> std::enable_if_t<M>
    {
        // channel is cccc in 0bxxxxcccc of the first byte
        this->m_message_data[0] = (this->m_message_data[0] & 0xf0) | (ch & 0x0f);
    }

    std::variant<NoteOffMessageView<Mutable>,
                 NoteOnMessageView<Mutable>,
                 PolyKeyPressureMessageView<Mutable>,
                 ControlChangeMessageView<Mutable>,
                 ChannelModeMessageView<Mutable>,
                 ProgramChangeMessageView<Mutable>,
                 ChannelPressureMessageView<Mutable>,
                 PitchBendMessageView<Mutable>,
                 AllSoundOffMessageView,
                 ResetAllControllersMessageView,
                 LocalControlMessageView<Mutable>,
                 AllNotesOffMessageView,
                 OmniModeOffMessageView,
                 OmniModeOnMessageView,
                 MonoModeOnMessageView<Mutable>,
                 PolyModeOnMessageView,
                 ChannelMessageView<Mutable>>
    parse() const
    {
        switch (this->m_message_data[0] & 0xf0)
        {
        case 0x80:
            return NoteOffMessageView<Mutable>(this->m_message_data);
        case 0x90:
            if (this->m_message_data[2] > 0)
            {
                return NoteOnMessageView<Mutable>(this->m_message_data);
            }
            else
            {
                return NoteOffMessageView<Mutable>(this->m_message_data);
            }
        case 0xa0:
            return PolyKeyPressureMessageView<Mutable>(this->m_message_data);
        case 0xb0:
            if (this->m_message_data[1] < 120)
            {
                return ControlChangeMessageView<Mutable>(this->m_message_data);
            }
            else
            {
                return utils::variant_cast(
                    ChannelModeMessageView<Mutable>(this->m_message_data).parse());
            }
        case 0xc0:
            return ProgramChangeMessageView<Mutable>(this->m_message_data);
        case 0xd0:
            return ChannelPressureMessageView<Mutable>(this->m_message_data);
        case 0xe0:
            return PitchBendMessageView<Mutable>(this->m_message_data);
        default:
            return *this;
        }
    }
};

struct NoteMessageViewTag
{
};

template <bool Mutable = true>
class NoteMessageView : public ChannelMessageView<Mutable>
{
public:
    using tag_t = NoteMessageViewTag;

    explicit NoteMessageView(MessageView<Mutable>::span_t message_data)
        : ChannelMessageView<Mutable>(message_data)
    {
        assert(message_data.size() == 3);
    }

    unsigned char get_note() const { return this->m_message_data[1]; }

    template <bool M = Mutable>
    auto set_note(unsigned char val) -> std::enable_if_t<M>
    {
        assert(val < 128);
        this->m_message_data[1] = val;
    }

    unsigned char get_velocity() const { return this->m_message_data[2]; }

    template <bool M = Mutable>
    auto set_velocity(unsigned char val) -> std::enable_if_t<M>
    {
        assert(val < 128);
        this->m_message_data[2] = val;
    }
};

struct NoteOffMessageViewTag : public NoteMessageViewTag
{
};

template <bool Mutable = true>
class NoteOffMessageView : public NoteMessageView<Mutable>
{
public:
    using tag_t = NoteOffMessageViewTag;
    explicit NoteOffMessageView(MessageView<Mutable>::span_t message_data)
        : NoteMessageView<Mutable>(message_data)
    {
    }
};

struct NoteOnMessageViewTag : public NoteMessageViewTag
{
};

template <bool Mutable = true>
class NoteOnMessageView : public NoteMessageView<Mutable>
{
public:
    using tag_t = NoteOnMessageViewTag;
    explicit NoteOnMessageView(MessageView<Mutable>::span_t message_data)
        : NoteMessageView<Mutable>(message_data)
    {
    }
};

struct PolyKeyPressureMessageViewTag : public ChannelMessageViewTag
{
};

template <bool Mutable = true>
class PolyKeyPressureMessageView : public ChannelMessageView<Mutable>
{
public:
    using tag_t = PolyKeyPressureMessageViewTag;
    explicit PolyKeyPressureMessageView(MessageView<Mutable>::span_t message_data)
        : ChannelMessageView<Mutable>(message_data)
    {
        assert(message_data.size() == 3);
    }

    unsigned char get_note() const { return this->m_message_data[1]; }

    template <bool M = Mutable>
    auto set_note(unsigned char val) -> std::enable_if_t<M>
    {
        assert(val < 128);
        this->m_message_data[1] = val;
    }

    unsigned char get_pressure() const { return this->m_message_data[2]; }

    template <bool M = Mutable>
    auto set_pressure(unsigned char val) -> std::enable_if_t<M>
    {
        assert(val < 128);
        this->m_message_data[2] = val;
    }
};

struct ControlChangeMessageViewTag : public ChannelMessageViewTag
{
};

template <bool Mutable = true>
class ControlChangeMessageView : public ChannelMessageView<Mutable>
{
public:
    using tag_t = ControlChangeMessageViewTag;
    explicit ControlChangeMessageView(MessageView<Mutable>::span_t message_data)
        : ChannelMessageView<Mutable>(message_data)
    {
    }

    unsigned char get_controller() const { return this->m_message_data[1]; }

    template <bool M = Mutable>
    auto set_controller(unsigned char val) -> std::enable_if_t<M>
    {
        assert(val < 128);
        this->m_message_data[1] = val;
    }

    unsigned char get_value() const { return this->m_message_data[2]; }

    template <bool M = Mutable>
    auto set_value(unsigned char val) -> std::enable_if_t<M>
    {
        assert(val < 128);
        this->m_message_data[2] = val;
    }

    std::string_view get_function_name() const
    {
        using namespace std::string_view_literals;
        switch (get_controller())
        {
        case 0:
            return "Bank Select"sv;
        case 1:
            return "Modulation Wheel"sv;
        case 2:
            return "Breath Controller"sv;
        case 4:
            return "Foot Controller"sv;
        case 5:
            return "Portamento Time"sv;
        case 6:
            return "Data Entry MSB"sv;
        case 7:
            return "Channel Volume"sv;
        case 8:
            return "Balance"sv;
        case 10:
            return "Pan"sv;
        case 11:
            return "Expression"sv;
        case 12:
            return "Effect Control 1"sv;
        case 13:
            return "Effect Control 2"sv;
        case 16:
            return "General Purpose Controller 1"sv;
        case 17:
            return "General Purpose Controller 2"sv;
        case 18:
            return "General Purpose Controller 3"sv;
        case 19:
            return "General Purpose Controller 4"sv;
        case 32 + 0:
            return "Bank Select (LSB)"sv;
        case 32 + 1:
            return "Modulation Wheel (LSB)"sv;
        case 32 + 2:
            return "Breath Controller (LSB)"sv;
        case 32 + 4:
            return "Foot Controller (LSB)"sv;
        case 32 + 5:
            return "Portamento Time (LSB)"sv;
        case 32 + 6:
            return "Data Entry LSB"sv;
        case 32 + 7:
            return "Channel Volume (LSB)"sv;
        case 32 + 8:
            return "Balance (LSB)"sv;
        case 32 + 10:
            return "Pan (LSB)"sv;
        case 32 + 11:
            return "Expression (LSB)"sv;
        case 32 + 12:
            return "Effect Control 1 (LSB)"sv;
        case 32 + 13:
            return "Effect Control 2 (LSB)"sv;
        case 32 + 16:
            return "General Purpose Controller 1 (LSB)"sv;
        case 32 + 17:
            return "General Purpose Controller 2 (LSB)"sv;
        case 32 + 18:
            return "General Purpose Controller 3 (LSB)"sv;
        case 32 + 19:
            return "General Purpose Controller 4 (LSB)"sv;
        case 64:
            return "Damper Pedal On/Off"sv;
        case 65:
            return "Portamento On/Off"sv;
        case 66:
            return "Sostenuto On/Off"sv;
        case 67:
            return "Soft Pedal On/Off"sv;
        case 68:
            return "Legato Footswitch"sv;
        case 69:
            return "Hold 2"sv;
        case 70:
            return "Sound Controller 1"sv;
        case 71:
            return "Sound Controller 2"sv;
        case 72:
            return "Sound Controller 3"sv;
        case 73:
            return "Sound Controller 4"sv;
        case 74:
            return "Sound Controller 5"sv;
        case 75:
            return "Sound Controller 6"sv;
        case 76:
            return "Sound Controller 7"sv;
        case 77:
            return "Sound Controller 8"sv;
        case 78:
            return "Sound Controller 9"sv;
        case 79:
            return "Sound Controller 10"sv;
        case 80:
            return "General Purpose Controller 5"sv;
        case 81:
            return "General Purpose Controller 6"sv;
        case 82:
            return "General Purpose Controller 7"sv;
        case 83:
            return "General Purpose Controller 8"sv;
        case 84:
            return "Portamento Control"sv;
        case 88:
            return "High Resolution Velocity Prefix"sv;
        case 91:
            return "Effects 1 Depth"sv;
        case 92:
            return "Effects 2 Depth"sv;
        case 93:
            return "Effects 3 Depth"sv;
        case 94:
            return "Effects 4 Depth"sv;
        case 95:
            return "Effects 5 Depth"sv;
        case 96:
            return "Data Increment"sv;
        case 97:
            return "Data Decrement"sv;
        case 98:
            return "Non-Registered Parameter Number LSB"sv;
        case 99:
            return "Non-Registered Parameter Number MSB"sv;
        case 100:
            return "Registered Parameter Number LSB"sv;
        case 101:
            return "Registered Parameter Number MSB"sv;
        default:
            return "Undefined"sv;
        }
    }
};

struct ChannelModeMessageViewTag : public ChannelMessageViewTag
{
};

template <bool Mutable = true>
class ChannelModeMessageView : public ChannelMessageView<Mutable>
{
public:
    using tag_t = ChannelModeMessageViewTag;
    explicit ChannelModeMessageView(MessageView<Mutable>::span_t message_data)
        : ChannelMessageView<Mutable>(message_data)
    {
    }

    std::variant<AllSoundOffMessageView,
                 ResetAllControllersMessageView,
                 LocalControlMessageView<Mutable>,
                 AllNotesOffMessageView,
                 OmniModeOffMessageView,
                 OmniModeOnMessageView,
                 MonoModeOnMessageView<Mutable>,
                 PolyModeOnMessageView,
                 ChannelModeMessageView>
    parse() const
    {
        switch (this->m_message_data[1])
        {
        case 120:
            return AllSoundOffMessageView(this->m_message_data);
        case 121:
            return ResetAllControllersMessageView(this->m_message_data);
        case 122:
            return LocalControlMessageView<Mutable>(this->m_message_data);
        case 123:
            return AllNotesOffMessageView(this->m_message_data);
        case 124:
            return OmniModeOffMessageView(this->m_message_data);
        case 125:
            return OmniModeOnMessageView(this->m_message_data);
        case 126:
            return MonoModeOnMessageView<Mutable>(this->m_message_data);
        case 127:
            return PolyModeOnMessageView(this->m_message_data);
        default:
            return *this;
        }
    }
};

struct AllSoundOffMessageViewTag : public ChannelModeMessageViewTag
{
};

class AllSoundOffMessageView : public ChannelModeMessageView<false>
{
public:
    using tag_t = AllSoundOffMessageViewTag;

    explicit AllSoundOffMessageView(MessageView<false>::span_t message_data)
        : ChannelModeMessageView<false>(message_data)
    {
    }
};

struct ResetAllControllersMessageViewTag : public ChannelModeMessageViewTag
{
};

class ResetAllControllersMessageView : public ChannelModeMessageView<false>
{
public:
    using tag_t = ResetAllControllersMessageViewTag;

    explicit ResetAllControllersMessageView(MessageView<false>::span_t message_data)
        : ChannelModeMessageView<false>(message_data)
    {
    }
};

struct LocalControlMessageViewTag : public ChannelModeMessageViewTag
{
};

template <bool Mutable>
class LocalControlMessageView : public ChannelModeMessageView<Mutable>
{
public:
    using tag_t = LocalControlMessageViewTag;

    explicit LocalControlMessageView(MessageView<Mutable>::span_t message_data)
        : ChannelModeMessageView<Mutable>(message_data)
    {
    }

    bool get_value() const { return this->m_message_data[2]; }

    template <bool M = Mutable>
    auto set_value(bool value) -> std::enable_if_t<M>
    {
        this->m_message_data[2] = value ? 127 : 0;
    }
};

struct AllNotesOffMessageViewTag : public ChannelModeMessageViewTag
{
};

class AllNotesOffMessageView : public ChannelModeMessageView<false>
{
public:
    using tag_t = AllNotesOffMessageViewTag;

    explicit AllNotesOffMessageView(MessageView<false>::span_t message_data)
        : ChannelModeMessageView<false>(message_data)
    {
    }
};

struct OmniModeOffMessageViewTag : public ChannelModeMessageViewTag
{
};

class OmniModeOffMessageView : public ChannelModeMessageView<false>
{
public:
    using tag_t = OmniModeOffMessageViewTag;

    explicit OmniModeOffMessageView(MessageView<false>::span_t message_data)
        : ChannelModeMessageView<false>(message_data)
    {
    }
};

struct OmniModeOnMessageViewTag : public ChannelModeMessageViewTag
{
};

class OmniModeOnMessageView : public ChannelModeMessageView<false>
{
public:
    using tag_t = OmniModeOnMessageViewTag;

    explicit OmniModeOnMessageView(MessageView<false>::span_t message_data)
        : ChannelModeMessageView<false>(message_data)
    {
    }
};

struct MonoModeOnMessageViewTag : public ChannelModeMessageViewTag
{
};

template <bool Mutable>
class MonoModeOnMessageView : public ChannelModeMessageView<Mutable>
{
public:
    using tag_t = MonoModeOnMessageViewTag;

    explicit MonoModeOnMessageView(MessageView<Mutable>::span_t message_data)
        : ChannelModeMessageView<Mutable>(message_data)
    {
    }

    unsigned char get_num_channels() const { return this->m_message_data[2]; }

    template <bool M = Mutable>
    auto set_num_channels(unsigned char value) -> std::enable_if_t<M>
    {
        this->m_message_data[2] = value;
    }
};

struct PolyModeOnMessageViewTag : public ChannelModeMessageViewTag
{
};

class PolyModeOnMessageView : public ChannelModeMessageView<false>
{
public:
    using tag_t = PolyModeOnMessageViewTag;

    explicit PolyModeOnMessageView(MessageView<false>::span_t message_data)
        : ChannelModeMessageView<false>(message_data)
    {
    }
};

struct ProgramChangeMessageViewTag : public ChannelMessageViewTag
{
};

template <bool Mutable = true>
class ProgramChangeMessageView : public ChannelMessageView<Mutable>
{
public:
    using tag_t = ProgramChangeMessageViewTag;
    explicit ProgramChangeMessageView(MessageView<Mutable>::span_t message_data)
        : ChannelMessageView<Mutable>(message_data)
    {
    }

    unsigned char get_program_number() const { return this->m_message_data[1]; }

    template <bool M = Mutable>
    auto set_program_number(unsigned char val) -> std::enable_if_t<M>
    {
        assert(val < 128);
        this->m_message_data[1] = val;
    }
};

struct ChannelPressureMessageViewTag : public ChannelMessageViewTag
{
};

template <bool Mutable = true>
class ChannelPressureMessageView : public ChannelMessageView<Mutable>
{
public:
    using tag_t = ChannelPressureMessageViewTag;
    explicit ChannelPressureMessageView(MessageView<Mutable>::span_t message_data)
        : ChannelMessageView<Mutable>(message_data)
    {
        assert(message_data.size() == 2);
    }

    unsigned char get_pressure() const { return this->m_message_data[1]; }

    template <bool M = Mutable>
    auto set_pressure(unsigned char val) -> std::enable_if_t<M>
    {
        assert(val < 128);
        this->m_message_data[1] = val;
    }
};

struct PitchBendMessageViewTag : public ChannelMessageViewTag
{
};

template <bool Mutable = true>
class PitchBendMessageView : public ChannelMessageView<Mutable>
{
public:
    using tag_t = PitchBendMessageViewTag;
    explicit PitchBendMessageView(MessageView<Mutable>::span_t message_data)
        : ChannelMessageView<Mutable>(message_data)
    {
        assert(message_data.size() == 3);
    }

    std::uint16_t get_value() const
    {
        return this->m_message_data[1] + std::uint16_t{127} * this->m_message_data[2];
    }

    int get_value_human() const { return get_value() - 8128; }

    template <bool M = Mutable>
    auto set_value(std::uint16_t val) -> std::enable_if_t<M>
    {
        assert(val < 127 * 127);
        this->m_message_data[1] = static_cast<unsigned char>(val % 127);
        this->m_message_data[2] = static_cast<unsigned char>(val / 127);
    }
};

} // namespace mc::midi
