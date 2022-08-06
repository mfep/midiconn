#include "PresetManager.hpp"

#include <filesystem>
#include <fstream>

#include "nlohmann/json.hpp"
#include "portable-file-dialogs.h"
#include "spdlog/spdlog.h"

#include "ConfigFile.hpp"
#include "NodeEditor.hpp"

namespace
{

bool ends_with_dot_json(const std::string& path)
{
    std::string lower_path;
    std::transform(path.begin(), path.end(), std::back_inserter(lower_path), [](const auto& ch) {
        return std::tolower(ch);
    });
    return lower_path.size() > 5 && 0 == lower_path.compare(lower_path.size() - 5, 5, ".json");
}

} // namespace

namespace mc::display
{

PresetManager::PresetManager(const NodeEditor&  editor,
                             const NodeFactory& node_factory,
                             ConfigFile&        config,
                             const char*        exe_path)
    : m_node_factory(&node_factory), m_config(&config), m_exe_path(exe_path)
{
    editor.to_json(m_last_editor_state);
}

bool PresetManager::is_dirty(const NodeEditor& editor) const
{
    nlohmann::json current_editor_state;
    editor.to_json(current_editor_state);
    return current_editor_state != m_last_editor_state;
}

NodeEditor PresetManager::open_preset(const std::string& open_path)
{
    spdlog::info("Opening preset from path \"{}\"", open_path);
    std::ifstream  ifs(open_path);
    nlohmann::json j;
    ifs >> j;
    auto node_editor    = NodeEditor::from_json(*m_node_factory, j);
    m_last_editor_state = j;
    m_opened_path       = open_path;
    return node_editor;
}

void PresetManager::save_preset(const NodeEditor& editor)
{
    save_preset(editor, false);
}

void PresetManager::save_preset_as(const NodeEditor& editor)
{
    save_preset(editor, true);
}

std::optional<NodeEditor> PresetManager::try_loading_last_preset()
{
    const auto last_preset_path = m_config->get_last_preset_path();
    if (!last_preset_path.has_value())
    {
        return std::nullopt;
    }
    try
    {
        return open_preset(last_preset_path.value().string());
    }
    catch (std::exception& ex)
    {
        spdlog::info("Could not load previous preset at \"{}\". Reason: \"{}\"",
                     last_preset_path.value().string(),
                     ex.what());
        return std::nullopt;
    }
}

void PresetManager::try_saving_last_preset_path() const
{
    if (!m_opened_path.has_value())
    {
        return;
    }
    m_config->set_last_preset_path(m_opened_path.value());
}

void PresetManager::save_preset(const NodeEditor& editor, const bool save_as)
{
    std::string save_path;
    if (!save_as && m_opened_path.has_value())
    {
        save_path = m_opened_path.value();
    }
    else
    {
        save_path = pfd::save_file("Save preset", ".", {"JSON files (*.json)", "*.json"}).result();
    }
    if (!save_path.empty())
    {
        spdlog::info("Saving preset file to \"{}\"", save_path);
        if (!ends_with_dot_json(save_path))
        {
            save_path += ".json";
        }
        nlohmann::json j;
        editor.to_json(j);
        std::ofstream ofs(save_path);
        ofs << j << std::endl;
        m_last_editor_state = j;
        m_opened_path       = save_path;
    }
}

} // namespace mc::display
