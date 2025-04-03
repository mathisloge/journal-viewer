// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <expected>
#include <string>
namespace jrn
{
template <typename T>
using Result = std::expected<T, std::string>;

constexpr auto make_error_result(auto &&message)
{
    return std::unexpected{std::forward<decltype(message)>(message)};
}
} // namespace jrn
