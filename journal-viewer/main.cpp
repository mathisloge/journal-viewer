#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <fmt/chrono.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/sinks/ConsoleSink.h>
#include "journal_log_manager.hpp"

static SDL_Window *g_window{nullptr};
static SDL_Renderer *g_renderer{nullptr};

static jrn::JournalLogManager manager{
    "/home/mlogemann/win-shared/2440022/log/journal/57b87fedc58b04117371df071322d349"};
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    quill::Backend::start();
    quill::Logger *logger = quill::Frontend::create_or_get_logger(
        "root", quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1"));

    if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        return SDL_AppResult::SDL_APP_FAILURE;
    }

    Uint32 window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window *window = SDL_CreateWindow("Journal log viewer", 1280, 720, window_flags);
    if (window == nullptr)
    {
        return SDL_AppResult::SDL_APP_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, 1);
    if (renderer == nullptr)
    {
        return SDL_AppResult::SDL_APP_FAILURE;
    }

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking

    io.Fonts->AddFontFromFileTTF("/home/mlogemann/dev/journal-viewer/journal-viewer/SourceCodePro-Regular.ttf", 15.0f);
    io.Fonts->Build();

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    g_window = window;
    g_renderer = renderer;
    return SDL_AppResult::SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    if ((SDL_GetWindowFlags(g_window) & SDL_WINDOW_MINIMIZED) != 0U)
    {
        SDL_Delay(10);
        return SDL_AppResult::SDL_APP_CONTINUE;
    }
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    if (ImGui::Begin("LogViewer"))
    {
        if (ImGui::BeginTable("log_view",
                              3,
                              ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable |
                                  ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Time");
            ImGui::TableSetupColumn("Unit");
            ImGui::TableSetupColumn("Message");
            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin(10000);
            while (clipper.Step())
            {
                int count = clipper.DisplayStart;
                for (auto it = manager.begin(clipper.DisplayStart); not it.is_end() and count < clipper.DisplayEnd;
                     it++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, [prio = it->priority] {
                        switch (prio)
                        {
                        case jrn::Priority::emergency:
                            return IM_COL32(112, 2, 147, 255);
                        case jrn::Priority::alert:
                            return IM_COL32(147, 2, 105, 255);
                        case jrn::Priority::critical:
                            return IM_COL32(147, 2, 2, 255);
                        case jrn::Priority::error:
                            return IM_COL32(143, 21, 0, 255);
                        case jrn::Priority::warning:
                            return IM_COL32(138, 83, 0, 255);
                        case jrn::Priority::notice:
                            return IM_COL32(54, 106, 12, 255);
                        case jrn::Priority::info:
                            return IM_COL32(0, 96, 138, 255);
                        case jrn::Priority::debug:
                            return IM_COL32(58, 77, 85, 255);
                        }
                        return IM_COL32(0, 0, 0, 255);
                    }());
                    const auto formatted_time{fmt::format("{}", it->utc)};
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(formatted_time.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(it->unit.c_str());
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextUnformatted(it->message.c_str());
                    // ImGui::PopStyleColor();
                    count++;
                }
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();

    // Rendering
    ImGui::Render();
    SDL_SetRenderScale(g_renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    constexpr ImVec4 kClearColor{0.45F, 0.55F, 0.60F, 1.00F};
    SDL_SetRenderDrawColorFloat(g_renderer, kClearColor.x, kClearColor.y, kClearColor.z, kClearColor.w);
    SDL_RenderClear(g_renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), g_renderer);
    SDL_RenderPresent(g_renderer);

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
        return SDL_AppResult::SDL_APP_SUCCESS;
    }
    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event->window.windowID == SDL_GetWindowID(g_window))
    {
        return SDL_AppResult::SDL_APP_SUCCESS;
    }

    return SDL_AppResult::SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    if (g_renderer != nullptr)
    {
        SDL_DestroyRenderer(g_renderer);
    }
    if (g_window != nullptr)
    {
        SDL_DestroyWindow(g_window);
    }
}
