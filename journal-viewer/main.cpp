// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <fmt/core.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <journal/logger.hpp>
#include <nfd.hpp>
#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/sinks/ConsoleSink.h>
#include "app_state.hpp"

using AppStatePointer = std::unique_ptr<jrn::AppState>;

namespace
{
constexpr jrn::AppState &create_stable_state(jrn::SdlRenderer renderer, jrn::SdlWindow window)
{
    static jrn::AppState state{std::move(renderer), std::move(window)};
    return state;
}
constexpr jrn::AppState &from_appstate(void *appstate)
{
    jrn::AppState *state = static_cast<jrn::AppState *>(appstate);
    return *state;
}
} // namespace

DEFINE_LOGGER(root)

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    fmt::println(R"(
    journal-viewer Copyright (C) 2025 Mathis Logemann
    This program comes with ABSOLUTELY NO WARRANTY; for details use the about box in the help menu.
    This is free software, and you are welcome to redistribute it
    under certain conditions;
    )");
    jrn::setup_logger();

    if (NFD::Init() != nfdresult_t::NFD_OKAY)
    {
        QUILL_LOG_CRITICAL(l_root(), "Could not initialize file dialogs");
        return SDL_AppResult::SDL_APP_FAILURE;
    }

    if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        QUILL_LOG_CRITICAL(l_root(), "Could not initialize SDL");
        return SDL_AppResult::SDL_APP_FAILURE;
    }

    jrn::SdlWindow window{nullptr, SDL_DestroyWindow};
    Uint32 window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    window.reset(SDL_CreateWindow("Journal log viewer", 1280, 720, window_flags));
    if (window == nullptr)
    {
        QUILL_LOG_CRITICAL(l_root(), "Could not create a window");
        return SDL_AppResult::SDL_APP_FAILURE;
    }

    jrn::SdlRenderer renderer{nullptr, SDL_DestroyRenderer};
    renderer.reset(SDL_CreateRenderer(window.get(), nullptr));
    if (renderer == nullptr)
    {
        QUILL_LOG_CRITICAL(l_root(), "Could not create a renderer");
        return SDL_AppResult::SDL_APP_FAILURE;
    }
    SDL_SetRenderVSync(renderer.get(), 1);

    SDL_SetWindowPosition(window.get(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window.get());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window.get(), renderer.get());
    ImGui_ImplSDLRenderer3_Init(renderer.get());

    *appstate = std::addressof(create_stable_state(std::move(renderer), std::move(window)));
    QUILL_LOG_INFO(l_root(), "App initialized.");
    return SDL_AppResult::SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    auto &state{from_appstate(appstate)};
    if ((SDL_GetWindowFlags(state.window()) & SDL_WINDOW_MINIMIZED) != 0U)
    {
        SDL_Delay(50);
        return SDL_AppResult::SDL_APP_CONTINUE;
    }

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    state.draw();

    ImGui::ShowDemoWindow();

    // Rendering
    ImGui::Render();

    ImGuiIO &io = ImGui::GetIO();
    SDL_SetRenderScale(state.renderer(), io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    constexpr ImVec4 kClearColor{0.45F, 0.55F, 0.60F, 1.00F};
    SDL_SetRenderDrawColorFloat(state.renderer(), kClearColor.x, kClearColor.y, kClearColor.z, kClearColor.w);
    SDL_RenderClear(state.renderer());
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), state.renderer());
    SDL_RenderPresent(state.renderer());

    return SDL_AppResult::SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (ImGui_ImplSDL3_ProcessEvent(event))
    {
        return SDL_AppResult::SDL_APP_CONTINUE;
    }
    if (event->type == SDL_EVENT_QUIT)
    {
        QUILL_LOG_INFO(l_root(), "Received quit event...");
        return SDL_AppResult::SDL_APP_SUCCESS;
    }
    auto &state{from_appstate(appstate)};
    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event->window.windowID == SDL_GetWindowID(state.window()))
    {
        QUILL_LOG_INFO(l_root(), "Received window close event...");
        return SDL_AppResult::SDL_APP_SUCCESS;
    }

    return SDL_AppResult::SDL_APP_CONTINUE;
}

void SDL_AppQuit(void * /*appstate*/, SDL_AppResult /*result*/)
{
    NFD::Quit();
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}
