#pragma once
#include <optional>
#include <string>

#include "nlohmann/json.hpp"

namespace mc
{
namespace midi
{
class Engine;
}
namespace display
{

class NodeEditor;

class PresetManager final
{
public:
    PresetManager(const NodeEditor& editor, midi::Engine& midi_engine, const char* exe_path);

    bool                              is_dirty(const NodeEditor& editor) const;
    NodeEditor                        open_preset(const std::string& path);
    void                              save_preset(const NodeEditor& editor);
    void                              save_preset_as(const NodeEditor& editor);
    std::optional<NodeEditor>         try_loading_last_preset();
    void                              try_saving_last_preset_path() const;
    const std::optional<std::string>& get_opened_path() const { return m_opened_path; }

private:
    void save_preset(const NodeEditor& editor, const bool save_as);

    midi::Engine*              m_midi_engine;
    nlohmann::json             m_last_editor_state;
    std::optional<std::string> m_opened_path;
    const char*                m_exe_path;
};

} // namespace display
} // namespace mc
