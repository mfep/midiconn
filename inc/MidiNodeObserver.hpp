#pragma once
#include "MidiEngine.hpp"
#include "NodeObserver.hpp"

namespace mc
{

class MidiNodeObserver final : public display::NodeObserver
{
public:
    explicit MidiNodeObserver(midi::Engine* midi_engine);

private:
    void node_created(const midi::InputInfo& input_info) override;
    void node_created(const midi::OutputInfo& output_info) override;
    void node_destroyed(const midi::InputInfo& input_info) override;
    void node_destroyed(const midi::OutputInfo& output_info) override;
    void link_created(const midi::InputInfo& input_info, const midi::OutputInfo& output_info) override;
    void link_destroyed(const midi::InputInfo& input_info, const midi::OutputInfo& output_info) override;

    midi::Engine* m_midi_engine;
};

}
