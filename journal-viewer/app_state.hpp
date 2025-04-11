// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <entt/entity/registry.hpp>
#include <journal_info.hpp>
#include "about_window.hpp"
#include "journal_instance_handle.hpp"
#include "journal_log_window.hpp"
#include "sdl_pointers.hpp"
#include "window_facade.hpp"
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
    AboutWindow about_;
    JournalInstanceHandle handle_;
    std::unique_ptr<JournalInfo> journal_info_;
    entt::registry registry_;
    std::uint32_t window_unique_id_{};
    entt::view<entt::get_t<LogWindowComponent>> window_view_;
};
} // namespace jrn
