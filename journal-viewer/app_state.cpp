#include "app_state.hpp"
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include "journal_instance.hpp"

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
                auto journal_handle =
                    choose_directory().and_then([](auto &&path) { return JournalInstance::from_directory(path); });
                if (journal_handle.has_value())
                {
                    handle_ = std::move(journal_handle.value());
                    log_windows_.emplace_back(std::make_unique<JournalLogWindow>("LogView", handle_));
                }
            }
            if (handle_.valid() and ImGui::MenuItem("New log window", "Ctrl-F"))
            {
                log_windows_.emplace_back(
                    std::make_unique<JournalLogWindow>(std::format("LogView##{}", log_windows_.size()), handle_));
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::SetNextWindowBgAlpha(0.20F);
    if (ImGui::Begin("FPS",
                     nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                         ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove))
    {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0F / io.Framerate, io.Framerate);
    }
    ImGui::End();

    for (auto &&win : log_windows_)
    {
        win->draw();
    }
}

} // namespace jrn
