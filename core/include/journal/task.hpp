// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <asio/awaitable.hpp>

namespace jrn
{
template <typename T>
using task = asio::awaitable<T>;
}
