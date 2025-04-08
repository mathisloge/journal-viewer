// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <asio/any_io_executor.hpp>
#include <asio/strand.hpp>
#include "async.hpp"
#include "journal_log_manager.hpp"
#include "journal_lru_entry_chunk_cache_new.hpp"
namespace jrn
{

class AsyncJournalLogManager
{
  public:
    explicit AsyncJournalLogManager(asio::io_context &io_ctx, JournalInstanceHandle handle);
    void reset_filters();
    void enable_priority(Priority priority);
    void disable_priority(Priority priority);

    void add_filter_systemd_unit(std::string systemd_unit);
    void remove_filter_systemd_unit(const std::string &systemd_unit);

    task<std::uint64_t> calculate_cursor_index(std::string_view cursor);

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
        while (begin < end)
        {
            const auto entry = entry_cache_.get_entry(begin, [this](auto start) {
                if (fetching_)
                {
                    return;
                }
                asio::post(strand_, [this, start]() {
                    fetching_ = true;
                    fetch_chunk(start, start + 250);
                    fetching_ = false;
                });
            });
            predicate(entry);
            begin++;
        }
    }
    [[nodiscard]] std::uint64_t min_count_entries() const
    {
        return cache_.cached_count();
    }

  private:
    void apply_current_matches();
    void add_priority_match(Priority priority);
    void fetch_chunk(std::uint64_t begin, std::uint64_t end);
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
    asio::strand<asio::any_io_executor> strand_;
    systemd::Journal journal_;
    JournalCache cache_{journal_.get()};
    JournalLruEntryChunkCacheNew entry_cache_;
    std::uint8_t enabled_priorities_{std::numeric_limits<std::uint8_t>::max()};
    std::unordered_set<std::string> enabled_systemd_units_;

    bool fetching_{false};
};

} // namespace jrn
