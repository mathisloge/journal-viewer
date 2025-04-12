// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "journal_log_window.hpp"
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include "window_facade.hpp"

using namespace std::literals;

namespace jrn
{
namespace
{
constexpr auto priority_color(Priority priority)
{
    switch (priority)
    {
    case Priority::emergency: {
        constexpr auto kColor = IM_COL32(112, 2, 147, 255);
        return kColor;
    }
    case Priority::alert: {
        constexpr auto kColor = IM_COL32(147, 2, 105, 255);
        return kColor;
    }
    case Priority::critical: {
        constexpr auto kColor = IM_COL32(147, 2, 2, 255);
        return kColor;
    }
    case Priority::error: {
        constexpr auto kColor = IM_COL32(143, 21, 0, 255);
        return kColor;
    }
    case Priority::warning: {
        constexpr auto kColor = IM_COL32(138, 83, 0, 255);
        return kColor;
    }
    case Priority::notice: {
        constexpr auto kColor = IM_COL32(54, 106, 12, 255);
        return kColor;
    }
    case Priority::info: {
        constexpr auto kColor = IM_COL32(0, 96, 138, 255);
        return kColor;
    }
    case Priority::debug: {
        constexpr auto kColor = IM_COL32(58, 77, 85, 255);
        return kColor;
    }
    }
    constexpr auto kUnknownColor = IM_COL32(58, 77, 85, 255);
    return kUnknownColor;
}
} // namespace

JournalLogWindow::JournalLogWindow(entt::registry &registry,
                                   std::string title,
                                   JournalInstanceHandle handle,
                                   const JournalInfo &info)
    : registry_{registry}
    , title_{std::move(title)}
    , manager_{handle}
    , info_{info}
{}

void JournalLogWindow::draw_priority_filter(std::string_view title, const Priority priority)
{
    bool enabled{manager_.is_priority_enabled(priority)};
    ImGui::PushStyleColor(ImGuiCol_FrameBg, priority_color(priority));
    if (ImGui::Checkbox(title.cbegin(), &enabled))
    {
        enabled ? manager_.enable_priority(priority) : manager_.disable_priority(priority);
    }
    ImGui::PopStyleColor();
}

void JournalLogWindow::draw()
{
    if (!ImGui::Begin(title_.c_str(), &open_, ImGuiWindowFlags_MenuBar))
    {
        ImGui::End();
        return;
    }

    // Menu bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Priorities"))
        {
            draw_priority_filter("Emergency"sv, Priority::emergency);
            draw_priority_filter("Alert"sv, Priority::alert);
            draw_priority_filter("Critical"sv, Priority::critical);
            draw_priority_filter("Error"sv, Priority::error);
            draw_priority_filter("Warning"sv, Priority::warning);
            draw_priority_filter("Notice"sv, Priority::notice);
            draw_priority_filter("Info"sv, Priority::info);
            draw_priority_filter("Debug"sv, Priority::debug);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Systemd-Units"))
        {
            for (auto &&unit : info_.systemd_units())
            {
                ImGui::PushID(&unit);
                bool enabled{manager_.has_filter_systemd_unit(unit)};
                if (ImGui::Checkbox(unit.c_str(), &enabled))
                {
                    enabled ? manager_.add_filter_systemd_unit(unit) : manager_.remove_filter_systemd_unit(unit);
                }
                ImGui::PopID();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (ImGui::InputText("Search", &search_text_, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        fmt::println("highlight: {}", search_text_);
        manager_.update_highlighter_search_text(search_text_);
    }
    if (ImGui::InputText("Exclude", &exclude_text_, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        fmt::println("exclude: {}", exclude_text_);
        manager_.update_exclude_message_regex(exclude_text_);
    }

    constexpr ImGuiTableFlags kTableFlags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersV |
                                            ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable |
                                            ImGuiTableFlags_RowBg;

    if (ImGui::BeginTable("for_sort_specs_only", 3, kTableFlags, ImGui::GetContentRegionAvail()))
    {
        ImGui::TableSetupColumn("Time");
        ImGui::TableSetupColumn("Unit");
        ImGui::TableSetupColumn("Message");
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin(manager_.min_count_entries() + 1000);
        while (clipper.Step())
        {
            if (requested_scroll_index_.has_value() and clipper.ItemsHeight > 1.F) [[unlikely]]
            {
                ImGui::SetScrollY(clipper.ItemsHeight * static_cast<float>(requested_scroll_index_.value()));
                past_scroll_index_ = requested_scroll_index_;
                requested_scroll_index_ = std::nullopt;
            }
            manager_.for_each(clipper.DisplayStart, clipper.DisplayEnd, [this](auto &&index, auto &&entry) {
                draw_entry(index, entry);
            });
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void JournalLogWindow::draw_entry(int index, const JournalEntry &entry)
{
    ImGui::PushID(index);
    ImGui::TableNextRow();
    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, priority_color(entry.priority));

    auto sys_time = clock_cast<std::chrono::system_clock>(entry.utc);
    auto local_time = std::chrono::zoned_time{std::chrono::current_zone(), sys_time}.get_local_time();

    const auto formatted_time{fmt::format("{}", local_time)};
    ImGui::TableSetColumnIndex(0);
    if (entry.highlight or past_scroll_index_.has_value() and past_scroll_index_ == index) [[unlikely]]
    {
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        const ImVec2 p = ImGui::GetCursorScreenPos();
        draw_list->AddRectFilled(
            {p.x - 5, p.y}, ImVec2{p.x, p.y + ImGui::GetTextLineHeight()}, IM_COL32(234, 162, 33, 255));
    }
    const bool selected{selected_cursor_ == entry.cursor};
    const ImGuiSelectableFlags flags = selected ? ImGuiSelectableFlags_Highlight : ImGuiSelectableFlags_None;
    if (ImGui::Selectable(formatted_time.c_str(), false, flags | ImGuiSelectableFlags_SpanAllColumns))
    {
        registry_.ctx().get<pro::proxy<LogWindowFacade>>()->scroll_to_cursor(entry.cursor);
    }
    if (ImGui::BeginPopupContextItem())
    {
        selected_cursor_ = entry.cursor;
        if (ImGui::Selectable("Copy message"))
        {
            ImGui::SetClipboardText(entry.message.c_str());
        }
        if (ImGui::Selectable("Exclude message"))
        {
        }
        ImGui::EndPopup();
    }
    if (not ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId))
    {
        selected_cursor_.clear();
    }
    ImGui::TableSetColumnIndex(1);
    ImGui::TextUnformatted(entry.unit.c_str());
    ImGui::TableSetColumnIndex(2);
    ImGui::TextUnformatted(entry.message.c_str());
    ImGui::PopID();
}

void JournalLogWindow::scroll_to_cursor(std::string_view cursor)
{
    requested_scroll_index_ = manager_.calculate_cursor_index(cursor) + 1;
}
} // namespace jrn
