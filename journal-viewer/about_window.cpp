// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "about_window.hpp"
#include <cmrc/cmrc.hpp>
#include <fmt/format.h>
#include <imgui.h>
#include <version.hpp>

CMRC_DECLARE(jrn);

namespace jrn
{
AboutWindow::AboutWindow()
    : github_issue_link_{fmt::format("{}/issues/new", kGitRepository)}
{}

void AboutWindow::draw()
{
    if (not open_)
    {
        return;
    }
    if (!ImGui::Begin("About journal-viewer", &open_, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Version %s", kVersion.begin());
    ImGui::Text("Git-Sha %s", kGitSha.begin());
    ImGui::Text("Git-Ref %s", kGitRef.begin());

    if (ImGui::Button("Copy version info"))
    {
        const auto clipboard =
            fmt::format("* VERSION: `{}`\n* GIT_REF: `{}`\n* GIT_SHA: `{}`\n", kVersion, kGitRef, kGitSha);
        ImGui::SetClipboardText(clipboard.c_str());
    }

    ImGui::TextLinkOpenURL("Homepage", kGitRepository.cbegin());
    ImGui::SameLine();
    ImGui::TextLinkOpenURL("Create issue", github_issue_link_.c_str());

    ImGui::Separator();
    const auto embedded_fs = cmrc::jrn::get_filesystem();
    const auto licence = embedded_fs.open("LICENSE");
    ImGui::TextUnformatted(licence.cbegin(), licence.cend());

    ImGui::End();
}
} // namespace jrn
