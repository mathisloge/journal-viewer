#pragma once
#include "file_browser.hpp"
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
    std::vector<std::unique_ptr<JournalLogWindow>> log_windows_;
};
} // namespace jrn
