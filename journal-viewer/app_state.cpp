// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app_state.hpp"
#include <cmrc/cmrc.hpp>
#include <imgui.h>
#include "file_browser.hpp"
#include "journal_instance.hpp"
CMRC_DECLARE(jrn);

namespace jrn
{
AppState::AppState(SdlRenderer renderer, SdlWindow window)
    : renderer_{std::move(renderer)}
    , window_{std::move(window)}
{
    auto embedded_fs = cmrc::jrn::get_filesystem();

    ImGuiIO &io = ImGui::GetIO();
    // imgui takes ownership
    auto font_src = embedded_fs.open("journal-viewer/SourceCodePro-Regular.ttf");
    uint8_t *raw_font = new uint8_t[font_src.size()]; // NOLINT
    std::copy(font_src.begin(), font_src.end(), raw_font);
    io.Fonts->AddFontFromMemoryTTF(static_cast<void *>(raw_font), font_src.size(), 16.0F);

    io.Fonts->Build();
}

AppState::~AppState() = default;

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
                    journal_info_ = std::make_unique<JournalInfo>(handle_);
                    main_log_window_ = std::make_unique<JournalLogWindow>("Main log window", handle_, *journal_info_);
                }
            }
            if (handle_.valid() and ImGui::MenuItem("New log window", "Ctrl-F"))
            {
                log_windows_.emplace_back(std::make_unique<JournalLogWindow>(
                    std::format("LogView##{}", log_windows_.size()), handle_, *journal_info_));
            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("About", NULL, about_.open_handle());
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


    about_.draw();

    if (main_log_window_ != nullptr) [[likely]]
    {
        main_log_window_->draw();
    }

    for (auto &&win : log_windows_)
    {
        win->draw();
    }
}

} // namespace jrn
