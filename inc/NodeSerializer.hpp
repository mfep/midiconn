#pragma once
#include <memory>

#include "nlohmann/json_fwd.hpp"

namespace mc
{
namespace midi
{
class Engine;
}

class DisconnectedMidiInNode;
class MidiChannelNode;
class MidiInNode;
class MidiOutNode;
class Node;

class NodeSerializer final
{
public:
    NodeSerializer(midi::Engine& midi_engine);

    void serialize_node(nlohmann::json& j, const Node& node) const;

    void serialize_node(nlohmann::json& j, const MidiInNode& node) const;
    void serialize_node(nlohmann::json& j, const DisconnectedMidiInNode& node) const;
    void serialize_node(nlohmann::json& j, const MidiOutNode& node) const;
    void serialize_node(nlohmann::json& j, const MidiChannelNode& node) const;

    std::shared_ptr<Node> deserialize_node(const nlohmann::json& j) const;

private:
    midi::Engine* m_midi_engine;
};

}
