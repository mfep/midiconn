#pragma once
#include <filesystem>
#include <optional>

#include "nlohmann/json.hpp"

#include "MidiEngine.hpp"
#include "NodeEditor.hpp"

namespace mc
{
class ConfigFile;
class NodeFactory;

namespace display
{

struct Preset
{
    NodeEditor            m_node_editor;
    midi::MessageTypeMask m_message_type_mask;

    void          to_json(nlohmann::json& j) const;
    static Preset from_json(const NodeFactory& node_factory, const nlohmann::json& j);
};

class PresetManager final
{
public:
    PresetManager(const Preset& preset, const NodeFactory& node_factory, ConfigFile& config);

    bool                                        is_dirty(const Preset& preset) const;
    Preset                                      open_preset(const std::filesystem::path& path);
    void                                        save_preset(const Preset& editor);
    void                                        save_preset_as(const Preset& editor);
    std::optional<Preset>                       try_loading_last_preset();
    void                                        try_saving_last_preset_path() const;
    const std::optional<std::filesystem::path>& get_opened_path() const { return m_opened_path; }

private:
    void save_preset(const Preset& preset, const bool save_as);

    const NodeFactory*                   m_node_factory;
    ConfigFile*                          m_config;
    nlohmann::json                       m_last_editor_state;
    std::optional<std::filesystem::path> m_opened_path;
};

} // namespace display
} // namespace mc
