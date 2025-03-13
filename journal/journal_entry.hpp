#pragma once
#include <chrono>
#include <string_view>
#include <systemd/sd-id128.h>
#include <systemd/sd-journal.h>

namespace jrn
{
enum class Priority
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

} // namespace jrn
