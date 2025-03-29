#include "journal_log_window.hpp"
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <imgui.h>

namespace jrn
{
namespace
{
void draw_entry(const JournalEntry &entry);
}
JournalLogWindow::JournalLogWindow(std::string title, const std::filesystem::path &file_or_directory)
    : title_{std::move(title)}
    , manager_{file_or_directory}
{}

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
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Add 10000 items"))
            {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Close", NULL, false, open_))
            {
                open_ = false;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::PushItemWidth(ImGui::GetFontSize() * 10);

            // ImGui::SeparatorText("Contents");
            // ImGui::Checkbox("Show Type Overlay", &ShowTypeOverlay);
            // ImGui::Checkbox("Allow Sorting", &AllowSorting);
            //
            // ImGui::SeparatorText("Selection Behavior");
            // ImGui::Checkbox("Allow dragging unselected item", &AllowDragUnselected);
            // ImGui::Checkbox("Allow box-selection", &AllowBoxSelect);
            //
            // ImGui::SeparatorText("Layout");
            // ImGui::SliderFloat("Icon Size", &IconSize, 16.0f, 128.0f, "%.0f");
            // ImGui::SameLine();
            // ImGui::SliderInt("Icon Spacing", &IconSpacing, 0, 32);
            // ImGui::SliderInt("Icon Hit Spacing", &IconHitSpacing, 0, 32);
            // ImGui::Checkbox("Stretch Spacing", &StretchSpacing);
            ImGui::PopItemWidth();
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
            manager_.for_each(clipper.DisplayStart, clipper.DisplayEnd, draw_entry);
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

namespace
{
void draw_entry(const JournalEntry &entry)
{
    ImGui::TableNextRow();
    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, [prio = entry.priority] {
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
    const auto formatted_time{fmt::format("{}", entry.utc)};
    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted(formatted_time.c_str());
    ImGui::TableSetColumnIndex(1);
    ImGui::TextUnformatted(entry.unit.c_str());
    ImGui::TableSetColumnIndex(2);
    ImGui::TextUnformatted(entry.message.c_str());
}
} // namespace
} // namespace jrn
