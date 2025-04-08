// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "async_journal_log_manager.hpp"
#include <regex>
#include <asio/co_spawn.hpp>
#include <asio/io_context.hpp>
#include <asio/use_awaitable.hpp>
#include "journal_instance.hpp"

namespace jrn
{

AsyncJournalLogManager::AsyncJournalLogManager(asio::io_context &io_ctx, JournalInstanceHandle handle)
    : strand_{io_ctx.get_executor()}
    , journal_{handle->create()}
    , entry_cache_{1000, 250}
{
    apply_current_matches();
}

void AsyncJournalLogManager::reset_filters()
{
    asio::post(strand_, [this]() {
        enabled_priorities_ = std::numeric_limits<std::uint8_t>::max();
        enabled_systemd_units_.clear();
        sd_journal_flush_matches(journal_.get());
        cache_.build_initial_cache(journal_.get());
    });
}

void AsyncJournalLogManager::enable_priority(Priority priority)
{
    asio::post(strand_, [this, priority]() {
        if (is_priority_enabled(priority))
        {
            return;
        }
        enabled_priorities_ |= details::flag_of(priority);

        add_priority_match(priority);
        cache_.build_initial_cache(journal_.get());
    });
}

void AsyncJournalLogManager::disable_priority(Priority priority)
{
    asio::post(strand_, [this, priority]() {
        if (not is_priority_enabled(priority))
        {
            return;
        }
        enabled_priorities_ &= ~details::flag_of(priority);
        apply_current_matches();
    });
}

void AsyncJournalLogManager::add_filter_systemd_unit(std::string systemd_unit)
{
    asio::post(strand_, [this, systemd_unit = std::move(systemd_unit)]() {
        if (not enabled_systemd_units_.emplace(std::move(systemd_unit)).second)
        {
            return;
        }
        apply_current_matches();
    });
}

void AsyncJournalLogManager::remove_filter_systemd_unit(const std::string &systemd_unit)
{
    asio::post(strand_, [this, systemd_unit = std::move(systemd_unit)]() {
        if (not has_filter_systemd_unit(systemd_unit))
        {
            return;
        }
        enabled_systemd_units_.erase(systemd_unit);
        apply_current_matches();
    });
}

task<std::uint64_t> AsyncJournalLogManager::calculate_cursor_index(std::string_view cursor)
{

    co_await asio::post(strand_, asio::use_awaitable); // put work in strand context

    sd_journal_seek_cursor(journal_.get(), cursor.cbegin());
    sd_journal_next(journal_.get());
    const auto nearest_cursor = fetch_cursor(journal_.get());
    std::uint64_t index = 0;
    sd_journal_seek_head(journal_.get());
    for (; sd_journal_next(journal_.get()) > 0; ++index)
    {
        if (sd_journal_test_cursor(journal_.get(), nearest_cursor.c_str()) > 0)
        {
            break;
        }
    }
    co_return index;
}

void AsyncJournalLogManager::apply_current_matches()
{
    sd_journal_flush_matches(journal_.get());

    add_priority_match(Priority::emergency);
    add_priority_match(Priority::alert);
    add_priority_match(Priority::critical);
    add_priority_match(Priority::error);
    add_priority_match(Priority::warning);
    add_priority_match(Priority::notice);
    add_priority_match(Priority::info);
    add_priority_match(Priority::debug);

    for (auto &&unit : enabled_systemd_units_)
    {
        const auto filter = std::format("{}={}", kSystemdUnitKey, unit);
        sd_journal_add_match(journal_.get(), filter.c_str(), filter.size());
    }

    cache_.build_initial_cache(journal_.get());
}

void AsyncJournalLogManager::add_priority_match(Priority priority)
{
    if (not is_priority_enabled(priority))
    {
        return;
    }
    const auto prio_filter = std::format("{}={}", kPriorityKey, std::to_underlying(priority));
    sd_journal_add_match(journal_.get(), prio_filter.c_str(), prio_filter.size());
}

bool AsyncJournalLogManager::match_dynamic_filter(const JournalEntry &entry) const
{
    std::regex self_regex("REGULAR EXPRESSIONS", std::regex_constants::ECMAScript | std::regex_constants::icase);
    return std::regex_search(entry.message, self_regex);
}

void AsyncJournalLogManager::fetch_chunk(std::uint64_t begin, const std::uint64_t end)
{
    const auto start_index = begin;
    std::vector<JournalEntry> entries;
    entries.reserve(250);
    cache_.seek_to_index(begin);
    while (begin < end)
    {
        auto entry = fetch_entry(journal_.get());
        entry.index = begin;
        if (not match_dynamic_filter(entry))
        {
            entries.emplace_back(std::move(entry));
            begin++;
        }
        if (sd_journal_next(journal_.get()) == 0)
        {
            break;
        }
    }
    entry_cache_.emplace_chunk(start_index, std::move(entries));
}
} // namespace jrn
