#include "PresetManager.hpp"

#include <filesystem>
#include <fstream>

#include "nlohmann/json.hpp"
#include "portable-file-dialogs.h"
#include "spdlog/spdlog.h"

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

PresetManager::PresetManager(const NodeEditor& editor,
                             midi::Engine&     midi_engine,
                             const char*       exe_path)
    : m_midi_engine(&midi_engine), m_exe_path(exe_path)
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
    auto node_editor    = NodeEditor::from_json(*m_midi_engine, j);
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
    std::string    previous_preset_path;
    nlohmann::json j;
    try
    {
        std::filesystem::path exe_fs_path(m_exe_path);
        const auto            json_path = exe_fs_path.replace_extension("json");
        spdlog::info("Loading config file from \"{}\"", json_path.string());
        {
            std::ifstream ifs(json_path);
            ifs >> j;
        }
        j["previous_preset_path"].get_to(previous_preset_path);
    }
    catch (std::exception& ex)
    {
        spdlog::info("Could not load config file. Reason: \"{}\"", ex.what());
        return std::nullopt;
    }
    try
    {
        return open_preset(previous_preset_path);
    }
    catch (std::exception& ex)
    {
        spdlog::info("Could not load previous preset at \"{}\". Reason: \"{}\"",
                     previous_preset_path,
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
    std::filesystem::path exe_fs_path(m_exe_path);
    const auto            json_path = exe_fs_path.replace_extension("json");
    spdlog::info("Saving config file to \"{}\"", json_path.string());
    try
    {
        nlohmann::json j;
        j["previous_preset_path"] = m_opened_path.value();
        std::ofstream ofstream(json_path);
        ofstream << j;
    }
    catch (std::exception& ex)
    {
        spdlog::warn("Could not save previous preset. Reason: \"{}\"", ex.what());
    }
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
