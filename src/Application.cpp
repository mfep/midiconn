#include "Application.hpp"
#include "imgui.h"
#include <SDL2/SDL.h>
#include "MidiEngine.hpp"
#include "NodeEditor.hpp"
#include "Version.hpp"

namespace mc::display
{

Application::Application() :
    m_is_done(false),
    m_midi_engine(std::make_unique<midi::Engine>()),
    m_node_editor(std::make_unique<NodeEditor>(*m_midi_engine))
{
}

Application::~Application() = default;

void Application::render()
{
    auto* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::Begin("Another Window", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);

    render_main_menu();
    try
    {
        m_node_editor->render();
    }
    catch(std::exception& ex)
    {
        std::cout << "Error: " << ex.what() << std::endl;
    }

    ImGui::End();
    ImGui::ShowDemoWindow();
}

void Application::render_main_menu()
{
    bool open_about_popup = false;
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                m_is_done = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                open_about_popup = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    if (open_about_popup)
    {
        ImGui::OpenPopup("About " MIDI_APPLICATION_NAME);
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("About " MIDI_APPLICATION_NAME, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(MC_MAJOR_VERSION "." MC_MINOR_VERSION "." MC_PATCH_VERSION "." MC_BUILD_NUMBER);
        ImGui::TextUnformatted(MC_COMMIT_HASH);
        ImGui::TextUnformatted(MC_BUILD_OS);
        if (ImGui::Button("Visit Website"))
        {
            SDL_OpenURL("https://mfeproject.itch.io");
        }
        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

bool Application::is_done() const
{
    return m_is_done;
}

}
