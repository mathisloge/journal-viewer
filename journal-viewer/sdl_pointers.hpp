// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <memory>
#include <SDL3/SDL_render.h>

namespace jrn
{
using SdlRenderer = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
using SdlWindow = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
} // namespace jrn
