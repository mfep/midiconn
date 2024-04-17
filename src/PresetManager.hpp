#pragma once
#include <filesystem>
#include <optional>

#include "nlohmann/json.hpp"

#include "NodeEditor.hpp"
#include "midi/MessageTypeMask.hpp"

namespace mc
{
class ConfigFile;
class NodeFactory;
class PortNameDisplay;
class ThemeControl;

struct Preset
{
    NodeEditor            m_node_editor;
    midi::MessageTypeMask m_message_type_mask;

    void          to_json(nlohmann::json& j) const;
    static Preset from_json(NodeFactory&           node_factory,
                            const PortNameDisplay& port_name_display,
                            const ThemeControl&    theme_control,
                            const nlohmann::json&  j);
};

class PresetManager final
{
public:
    PresetManager(const Preset&          preset,
                  NodeFactory&           node_factory,
                  ConfigFile&            config,
                  const PortNameDisplay& port_name_display,
                  const ThemeControl&    theme_control);

    bool                                        is_dirty(const Preset& preset) const;
    Preset                                      open_preset(const std::filesystem::path& path);
    void                                        save_preset(const Preset& editor);
    void                                        save_preset_as(const Preset& editor);
    std::optional<Preset>                       try_loading_last_preset();
    void                                        try_saving_last_preset_path() const;
    const std::optional<std::filesystem::path>& get_opened_path() const { return m_opened_path; }

private:
    void save_preset(const Preset& preset, const bool save_as);

    NodeFactory*                         m_node_factory;
    ConfigFile*                          m_config;
    nlohmann::json                       m_last_editor_state;
    std::optional<std::filesystem::path> m_opened_path;
    const PortNameDisplay*               m_port_name_display;
    const ThemeControl*                  m_theme_control;
};

} // namespace mc
