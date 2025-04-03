// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <unordered_set>
#include <utility>
#include "details/sd_journal.hpp"
#include "journal_cache.hpp"
#include "journal_entry.hpp"
#include "journal_instance_handle.hpp"

namespace jrn
{
namespace details
{
inline constexpr std::array<std::uint8_t, 8> kPrioritiesFlags{
    1, (1 << 1), (1 << 2), (1 << 3), (1 << 4), (1 << 5), (1 << 6), (1 << 7)};
constexpr bool is_flag_set(Priority priority, const std::uint8_t enabled_priorities)
{
    return (enabled_priorities & kPrioritiesFlags[std::to_underlying(priority)]) != 0;
}

constexpr std::uint8_t flag_of(Priority priority)
{
    return kPrioritiesFlags[std::to_underlying(priority)];
}
} // namespace details

class JournalLogManager
{
  public:
    explicit JournalLogManager(JournalInstanceHandle handle);

    void reset_filters();
    void enable_priority(Priority priority);
    void disable_priority(Priority priority);

    void add_filter_systemd_unit(std::string systemd_unit);
    void remove_filter_systemd_unit(const std::string &systemd_unit);
    [[nodiscard]] bool has_filter_systemd_unit(const std::string &systemd_unit) const
    {
        return enabled_systemd_units_.contains(systemd_unit);
    }

    [[nodiscard]] bool is_priority_enabled(const Priority priority) const
    {
        return details::is_flag_set(priority, enabled_priorities_);
    }

    void for_each(std::uint64_t begin, const std::uint64_t end, auto &&predicate)
    {
        cache_.seek_to_index(begin);
        while (begin < end)
        {
            const auto entry = fetch_entry(journal_.get());
            if (not match_dynamic_filter(entry))
            {
                predicate(entry);
                begin++;
            }
            if (sd_journal_next(journal_.get()) == 0)
            {
                break;
            }
        }
    }

    std::uint64_t min_count_entries() const
    {
        return cache_.cached_count();
    }

  private:
    void apply_current_matches();
    void add_priority_match(Priority priority);

    /**
     * @brief Applies dynamic filters which cannot be applied to systemd filters. E.g. regex message filters, excluding
     * messages etc.
     *
     * @param entry the entry to match
     * @return true if any filter applies
     * @return false if non of the filter applies
     */
    bool match_dynamic_filter(const JournalEntry &entry) const;

  private:
    systemd::Journal journal_;
    JournalCache cache_{journal_.get()};
    std::uint8_t enabled_priorities_{std::numeric_limits<std::uint8_t>::max()};
    std::unordered_set<std::string> enabled_systemd_units_;
};
} // namespace jrn
