// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "journal_log_window.hpp"
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <imgui.h>

using namespace std::literals;

namespace jrn
{
namespace
{
constexpr auto priority_color(Priority priority)
{
    switch (priority)
    {
    case Priority::emergency:
        return IM_COL32(112, 2, 147, 255);
    case Priority::alert:
        return IM_COL32(147, 2, 105, 255);
    case Priority::critical:
        return IM_COL32(147, 2, 2, 255);
    case Priority::error:
        return IM_COL32(143, 21, 0, 255);
    case Priority::warning:
        return IM_COL32(138, 83, 0, 255);
    case Priority::notice:
        return IM_COL32(54, 106, 12, 255);
    case Priority::info:
        return IM_COL32(0, 96, 138, 255);
    case Priority::debug:
        return IM_COL32(58, 77, 85, 255);
    }
    return IM_COL32(0, 0, 0, 255);
}
} // namespace

JournalLogWindow::JournalLogWindow(std::string title, JournalInstanceHandle handle, const JournalInfo &info)
    : title_{std::move(title)}
    , manager_{handle}
    , info_{info}
{}

void JournalLogWindow::draw_priority_filter(std::string_view title, const Priority priority)
{
    bool enabled{manager_.is_priority_enabled(priority)};
    ImGui::PushStyleColor(ImGuiCol_FrameBg, priority_color(priority));
    if (ImGui::Checkbox(title.data(), &enabled))
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
    constexpr ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp |
                                            ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("for_sort_specs_only", 3, table_flags, ImGui::GetContentRegionAvail()))
    {
        ImGui::TableSetupColumn("Time");
        ImGui::TableSetupColumn("Unit");
        ImGui::TableSetupColumn("Message");
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin(manager_.min_count_entries());
        while (clipper.Step())
        {
            manager_.for_each(clipper.DisplayStart, clipper.DisplayEnd, [this](auto &&entry) { draw_entry(entry); });
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void JournalLogWindow::draw_entry(const JournalEntry &entry)
{
    ImGui::TableNextRow();
    ImGui::PushID(&entry);
    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, priority_color(entry.priority));
    const auto formatted_time{fmt::format("{}", entry.utc)};
    ImGui::TableSetColumnIndex(0);
    const bool selected{selected_cursor_ == entry.cursor};
    const ImGuiSelectableFlags flags = selected ? ImGuiSelectableFlags_Highlight : ImGuiSelectableFlags_None;
    ImGui::Selectable(formatted_time.c_str(), false, flags | ImGuiSelectableFlags_SpanAllColumns);
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
} // namespace jrn
