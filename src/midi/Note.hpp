#pragma once

#include "fmt/format.h"

#include <sstream>
#include <string_view>

namespace mc::midi
{

class Note final
{
public:
    explicit Note(unsigned char note) : m_note(note) {}

private:
    unsigned char m_note;

    friend struct fmt::formatter<Note>;
};

} // namespace mc::midi

template <>
struct fmt::formatter<mc::midi::Note>
{
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <class FmtContext>
    FmtContext::iterator format(mc::midi::Note note, FmtContext& ctx) const
    {
        using namespace std::string_view_literals;
        const auto key = [=] {
            switch (note.m_note % 12)
            {
            case 0:
                return "C"sv;
            case 1:
                return "C#"sv;
            case 2:
                return "D"sv;
            case 3:
                return "D#"sv;
            case 4:
                return "E"sv;
            case 5:
                return "F"sv;
            case 6:
                return "F#"sv;
            case 7:
                return "G"sv;
            case 8:
                return "G#"sv;
            case 9:
                return "A"sv;
            case 10:
                return "A#"sv;
            case 11:
                return "B"sv;
            default:
                return ""sv;
            }
        }();
        const auto         octave = note.m_note / 12;
        std::ostringstream out;
        out << key << octave;
        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};
