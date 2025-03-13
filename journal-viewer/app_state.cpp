#include "app_state.hpp"
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

namespace jrn
{
AppState::AppState(SdlRenderer renderer, SdlWindow window)
    : renderer_{std::move(renderer)}
    , window_{std::move(window)}
{}

AppState::~AppState()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void AppState::draw()
{
    ImGuiIO &io = ImGui::GetIO();
    ImGui::DockSpaceOverViewport();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open directory", "Ctrl+D"))
            {
                auto path = choose_directory();
                path.and_then([this](auto &&path) {
                    main_log_window_ = std::make_unique<JournalLogWindow>("LogView", path);
                    return std::optional{path};
                });
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (ImGui::Begin("FPS",
                     nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                         ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove))
    {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    }
    ImGui::End();

    if (main_log_window_ != nullptr)
    {
        main_log_window_->draw();
    }
}

} // namespace jrn
