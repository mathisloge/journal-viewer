#include "journal_entry.hpp"
#include <charconv>

namespace jrn
{
namespace
{
std::string_view get_data(sd_journal *journal, std::string_view field);
Priority get_priority(sd_journal *journal);
UtcTimepoint get_realtime(sd_journal *journal);
} // namespace

JournalEntry fetch_entry(sd_journal *journal)
{
    JournalEntry entry;

    entry.message = get_data(journal, "MESSAGE");
    entry.unit = get_systemd_unit(journal);
    entry.priority = get_priority(journal);
    entry.utc = get_realtime(journal);

    return entry;
}

std::string get_systemd_unit(sd_journal *journal)
{
    return std::string{get_data(journal, kSystemdUnitKey)};
}

namespace
{
std::string_view get_data(sd_journal *journal, std::string_view field)
{
    const void *data{};
    size_t length{};
    std::ignore = sd_journal_get_data(journal, field.begin(), &data, &length);
    std::string_view data_str{static_cast<const char *>(data), length};
    const auto pos_prefix = data_str.find_first_of('=');
    if (pos_prefix != std::string_view::npos)
    {
        data_str = data_str.substr(pos_prefix + 1);
    }
    return data_str;
}

Priority get_priority(sd_journal *journal)
{
    auto priority_str = get_data(journal, kPriorityKey);
    std::uint8_t value = 0;
    std::from_chars(priority_str.begin(), priority_str.end(), value);
    return Priority{value};
}

UtcTimepoint get_realtime(sd_journal *journal)
{
    std::uint64_t realtime{};
    sd_journal_get_realtime_usec(journal, &realtime);
    return UtcTimepoint{std::chrono::microseconds{realtime}};
}
} // namespace
} // namespace jrn
