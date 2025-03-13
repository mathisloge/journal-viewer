#pragma once
#include <memory>
#include <SDL3/SDL_render.h>

namespace jrn
{
using SdlRenderer = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
using SdlWindow = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
}
