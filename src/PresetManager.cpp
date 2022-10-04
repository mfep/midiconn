#include "PresetManager.hpp"

#include <filesystem>
#include <fstream>

#include "nlohmann/json.hpp"
#include "portable-file-dialogs.h"
#include "spdlog/spdlog.h"

#include "ConfigFile.hpp"
#include "NodeEditor.hpp"
#include "Version.hpp"

namespace mc::midi
{
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MessageTypeMask,
                                   m_sysex_enabled,
                                   m_time_enabled,
                                   m_sensing_enabled);
}

namespace mc::display
{

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

void Preset::to_json(nlohmann::json& j) const
{
    m_node_editor.to_json(j["editor"]);
    j["enabled_message_types"] = m_message_type_mask;
}

Preset Preset::from_json(const NodeFactory& node_factory, const nlohmann::json& j)
{
    return {NodeEditor::from_json(node_factory, j.at("editor")), j.at("enabled_message_types")};
}

PresetManager::PresetManager(const Preset&      preset,
                             const NodeFactory& node_factory,
                             ConfigFile&        config)
    : m_node_factory(&node_factory), m_config(&config)
{
    preset.to_json(m_last_editor_state);
}

bool PresetManager::is_dirty(const Preset& preset) const
{
    nlohmann::json current_editor_state;
    preset.to_json(current_editor_state);
    return current_editor_state != m_last_editor_state;
}

Preset PresetManager::open_preset(const std::string& open_path)
{
    spdlog::info("Opening preset from path \"{}\"", open_path);
    std::ifstream ifs(open_path);
    ifs >> m_last_editor_state;
    m_opened_path = open_path;
    return Preset::from_json(*m_node_factory, m_last_editor_state);
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

void PresetManager::save_preset(const Preset& preset, const bool save_as)
{
    std::string save_path;
    if (!save_as && m_opened_path.has_value())
    {
        save_path = m_opened_path.value();
    }
    else
    {
        const auto filename =
            std::filesystem::path(m_opened_path.value_or("preset.json")).filename().string();
        save_path =
            pfd::save_file("Save preset", filename, {"JSON files (*.json)", "*.json"}).result();
    }
    if (!save_path.empty())
    {
        spdlog::info("Saving preset file to \"{}\"", save_path);
        if (!ends_with_dot_json(save_path))
        {
            save_path += ".json";
        }
        nlohmann::json j;
        preset.to_json(j);
        m_last_editor_state = j;
        j["version"]        = MC_FULL_VERSION;
        {
            std::ofstream ofs(save_path);
            ofs << j << std::endl;
        }
        m_opened_path = save_path;
    }
}

} // namespace mc::display
