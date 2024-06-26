#include "PresetManager.hpp"

#include <fstream>

#include "nlohmann/json.hpp"
#include "portable-file-dialogs.h"
#include "spdlog/spdlog.h"

#include "ConfigFile.hpp"
#include "NodeEditor.hpp"
#include "Utils.hpp"
#include "Version.hpp"

namespace mc
{

void Preset::to_json(nlohmann::json& j) const
{
    m_node_editor.to_json(j["editor"]);
}

Preset Preset::from_json(NodeFactory&           node_factory,
                         const PortNameDisplay& port_name_display,
                         const ThemeControl&    theme_control,
                         const nlohmann::json&  j)
{
    return {NodeEditor::from_json(node_factory, port_name_display, theme_control, j.at("editor"))};
}

PresetManager::PresetManager(const Preset&          preset,
                             NodeFactory&           node_factory,
                             ConfigFile&            config,
                             const PortNameDisplay& port_name_display,
                             const ThemeControl&    theme_control)
    : m_node_factory(&node_factory), m_config(&config), m_port_name_display(&port_name_display),
      m_theme_control(&theme_control)
{
    preset.to_json(m_last_editor_state);
}

bool PresetManager::is_dirty(const Preset& preset) const
{
    nlohmann::json current_editor_state;
    preset.to_json(current_editor_state);
    return current_editor_state != m_last_editor_state;
}

Preset PresetManager::open_preset(const std::filesystem::path& open_path)
{
    spdlog::info("Opening preset from path \"{}\"", utils::path_to_utf8str(open_path));
    std::ifstream ifs(open_path);
    if (!ifs.good())
    {
        throw std::runtime_error("Cannot open preset file at \"" +
                                 utils::path_to_utf8str(open_path) + "\"");
    }
    ifs >> m_last_editor_state;
    m_opened_path = open_path;
    return Preset::from_json(
        *m_node_factory, *m_port_name_display, *m_theme_control, m_last_editor_state);
}

void PresetManager::save_preset(const Preset& preset)
{
    save_preset(preset, false);
}

void PresetManager::save_preset_as(const Preset& preset)
{
    save_preset(preset, true);
}

std::optional<Preset> PresetManager::try_loading_last_preset()
{
    const auto last_preset_path = m_config->get_last_preset_path();
    if (!last_preset_path.has_value())
    {
        return std::nullopt;
    }
    try
    {
        return open_preset(last_preset_path.value());
    }
    catch (std::exception& ex)
    {
        spdlog::info("Could not load previous preset at \"{}\". Reason: \"{}\"",
                     utils::path_to_utf8str(last_preset_path.value()),
                     ex.what());
        throw;
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

void PresetManager::save_preset(const Preset& preset, const bool save_as)
{
    std::filesystem::path save_path;
    if (!save_as && m_opened_path.has_value())
    {
        save_path = m_opened_path.value();
    }
    else
    {
        const auto filename =
            std::filesystem::path(m_opened_path.value_or("preset.mcpreset")).filename();
        save_path =
            std::filesystem::u8path(pfd::save_file("Save preset",
                                                   utils::path_to_utf8str(filename),
                                                   {"midiconn presets (*.mcpreset)", "*.mcpreset"})
                                        .result());
    }
    if (!save_path.empty())
    {
        spdlog::info("Saving preset file to \"{}\"", utils::path_to_utf8str(save_path));
        if (save_path.extension() != ".mcpreset")
        {
            save_path += ".mcpreset";
        }
        nlohmann::json j;
        preset.to_json(j);
        m_last_editor_state = j;
        j["version"]        = g_current_version_str;
        {
            std::ofstream ofs(save_path);
            ofs << j << std::endl;
        }
        m_opened_path = save_path;
    }
}

} // namespace mc
