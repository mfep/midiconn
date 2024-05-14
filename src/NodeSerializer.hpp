#pragma once
#include <memory>

#include "nlohmann/json_fwd.hpp"

namespace mc
{

class DisconnectedMidiInNode;
class DisconnectedMidiOutNode;
class LogNode;
class MidiChannelNode;
class MidiInNode;
class MidiOutNode;
class Node;
class NodeFactory;

class NodeSerializer final
{
public:
    NodeSerializer(NodeFactory& node_factory);

    void serialize_node(nlohmann::json& j, const Node& node) const;

    void serialize_node(nlohmann::json& j, const DisconnectedMidiInNode& node) const;
    void serialize_node(nlohmann::json& j, const DisconnectedMidiOutNode& node) const;
    void serialize_node(nlohmann::json& j, const LogNode& node) const;
    void serialize_node(nlohmann::json& j, const MidiChannelNode& node) const;
    void serialize_node(nlohmann::json& j, const MidiInNode& node) const;
    void serialize_node(nlohmann::json& j, const MidiOutNode& node) const;

    std::shared_ptr<Node> deserialize_node(const nlohmann::json& j) const;

private:
    NodeFactory* m_node_factory;
};

} // namespace mc
