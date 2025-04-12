// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <chrono>
#include <string_view>
#include <systemd/sd-id128.h>
#include <systemd/sd-journal.h>

namespace jrn
{
inline constexpr std::string_view kSystemdUnitKey = "_SYSTEMD_UNIT";
inline constexpr std::string_view kPriorityKey = "PRIORITY";

enum class Priority : std::uint8_t
{
    emergency = 0,
    alert,
    critical,
    error,
    warning,
    notice,
    info,
    debug
};

using UtcTimepoint = std::chrono::utc_clock::time_point;

struct JournalEntry
{
    std::string cursor;
    std::string unit;
    std::string message;
    Priority priority{};
    UtcTimepoint utc;
    bool highlight{};
};

[[nodiscard]] JournalEntry fetch_entry(sd_journal *journal);
[[nodiscard]] std::string fetch_cursor(sd_journal *journal);
[[nodiscard]] std::string get_systemd_unit(sd_journal *journal);
[[nodiscard]] std::string_view extract_field_data(const void *data, size_t length);
} // namespace jrn
