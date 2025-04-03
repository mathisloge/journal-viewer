// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <string>
namespace jrn
{
class AboutWindow
{
  public:
    AboutWindow();
    void draw();
    bool *open_handle()
    {
        return &open_;
    }

  private:
    bool open_{false};
    std::string github_issue_link_;
};
} // namespace jrn
