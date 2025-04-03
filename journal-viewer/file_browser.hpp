// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <filesystem>
#include <result.hpp>

namespace jrn
{
Result<std::filesystem::path> choose_directory();
} // namespace jrn
