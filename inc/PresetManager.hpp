#pragma once
#include <optional>
#include <string>

#include "nlohmann/json.hpp"

namespace mc
{
class ConfigFile;
class NodeFactory;

namespace display
{

class NodeEditor;

class PresetManager final

{
public:
    PresetManager(const NodeEditor&  editor,
                  const NodeFactory& node_factory,
                  ConfigFile&        config,
                  const char*        exe_path);

    bool                              is_dirty(const NodeEditor& editor) const;
    NodeEditor                        open_preset(const std::string& path);
    void                              save_preset(const NodeEditor& editor);
    void                              save_preset_as(const NodeEditor& editor);
    std::optional<NodeEditor>         try_loading_last_preset();
    void                              try_saving_last_preset_path() const;
    const std::optional<std::string>& get_opened_path() const { return m_opened_path; }

private:
    void save_preset(const NodeEditor& editor, const bool save_as);

    const NodeFactory*         m_node_factory;
    ConfigFile*                m_config;
    nlohmann::json             m_last_editor_state;
    std::optional<std::string> m_opened_path;
    const char*                m_exe_path;
};

} // namespace display
} // namespace mc
