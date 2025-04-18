// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "file_browser.hpp"
#include <nfd.hpp>

namespace jrn
{
Result<std::filesystem::path> choose_directory()
{
    NFD::UniquePathN path;
    const auto result = NFD::PickFolder(path);
    if (result != NFD_OKAY)
    {
        return make_error_result("Filedialog aborted");
    }
    return std::filesystem::path{path.get()};
}
} // namespace jrn
