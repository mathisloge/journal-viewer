// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <journal_info.hpp>
#include "journal_instance_handle.hpp"
#include "journal_log_window.hpp"
#include "sdl_pointers.hpp"
namespace jrn
{
class AppState final
{
  public:
    explicit AppState(SdlRenderer renderer, SdlWindow window);
    ~AppState();

    void draw();

    SDL_Window *window()
    {
        return window_.get();
    }

    SDL_Renderer *renderer()
    {
        return renderer_.get();
    }

  private:
    SdlRenderer renderer_;
    SdlWindow window_;
    JournalInstanceHandle handle_;
    std::unique_ptr<JournalInfo> journal_info_;
    std::unique_ptr<JournalLogWindow> main_log_window_;
    std::vector<std::unique_ptr<JournalLogWindow>> log_windows_;
};
} // namespace jrn
