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

using UtcTimepoint = std::chrono::system_clock::time_point;

struct JournalEntry
{
    sd_id128_t id{};
    std::string unit;
    std::string message;
    Priority priority;
    UtcTimepoint utc;
};

JournalEntry fetch_entry(sd_journal *journal);

std::string get_systemd_unit(sd_journal *journal);
} // namespace jrn
