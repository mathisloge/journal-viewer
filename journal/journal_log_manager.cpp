// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "journal_log_manager.hpp"
#include <utility>
#include <journal/logger.hpp>
#include <quill/StopWatch.h>
#include "journal_instance.hpp"

DEFINE_LOGGER(log_manager);
namespace jrn
{
JournalLogManager::JournalLogManager(JournalInstanceHandle handle)
    : journal_{handle->create()}
{
    apply_current_matches();
}

void JournalLogManager::reset_filters()
{
    QUILL_LOG_DEBUG(l_log_manager(), "reset filters");
    enabled_priorities_ = std::numeric_limits<std::uint8_t>::max();
    enabled_systemd_units_.clear();
    sd_journal_flush_matches(journal_.get());
    cache_.build_initial_cache(journal_.get());
}

void JournalLogManager::enable_priority(Priority priority)
{
    if (is_priority_enabled(priority))
    {
        return;
    }
    QUILL_LOG_DEBUG(l_log_manager(), "enable priority {}", static_cast<int>(priority));
    enabled_priorities_ |= details::flag_of(priority);

    add_priority_match(priority);
    cache_.build_initial_cache(journal_.get());
}

void JournalLogManager::disable_priority(Priority priority)
{
    if (not is_priority_enabled(priority))
    {
        return;
    }
    QUILL_LOG_DEBUG(l_log_manager(), "disable priority {}", static_cast<int>(priority));
    enabled_priorities_ &= ~details::flag_of(priority);
    apply_current_matches();
}

void JournalLogManager::update_highlighter_search_text(std::string search_text)
{
    QUILL_LOG_DEBUG(l_log_manager(), "update search text to {}", search_text);
    highlighter_query_ = std::move(search_text);
}

void JournalLogManager::update_exclude_message_regex(std::string exclude_text)
{
    if (exclude_text.empty())
    {
        QUILL_LOG_DEBUG(l_log_manager(), "update exclude query to null");
        exclude_query_ = std::nullopt;
        return;
    }
    QUILL_LOG_DEBUG(l_log_manager(), "update exclude query to {}", exclude_text);
    exclude_query_ =
        std::regex{std::move(exclude_text), std::regex_constants::ECMAScript | std::regex_constants::optimize};
}

void JournalLogManager::add_filter_systemd_unit(std::string systemd_unit)
{
    QUILL_LOG_DEBUG(l_log_manager(), "add unit '{}' to filter", systemd_unit);
    if (not enabled_systemd_units_.emplace(std::move(systemd_unit)).second)
    {
        return;
    }
    apply_current_matches();
}

void JournalLogManager::remove_filter_systemd_unit(const std::string &systemd_unit)
{
    QUILL_LOG_DEBUG(l_log_manager(), "remove unit '{}' from filter", systemd_unit);
    if (not has_filter_systemd_unit(systemd_unit))
    {
        return;
    }
    enabled_systemd_units_.erase(systemd_unit);
    apply_current_matches();
}

void JournalLogManager::add_priority_match(Priority priority)
{
    if (not is_priority_enabled(priority))
    {
        return;
    }
    const auto prio_filter = std::format("{}={}", kPriorityKey, std::to_underlying(priority));
    sd_journal_add_match(journal_.get(), prio_filter.c_str(), prio_filter.size());
}

bool JournalLogManager::match_dynamic_filter(const JournalEntry &entry) const
{
    if (not exclude_query_.has_value())
    {
        return false;
    }
    return std::regex_search(entry.message, exclude_query_.value());
}

bool JournalLogManager::match_dynamic_highlighter(const JournalEntry &entry) const
{
    if (highlighter_query_.empty())
    {
        return false;
    }
    return entry.message.find(highlighter_query_) != std::string::npos;
}

void JournalLogManager::apply_current_matches()
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

std::uint64_t JournalLogManager::calculate_cursor_index(std::string_view cursor)
{
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
    return index;
}

std::vector<JournalEntry> JournalLogManager::fetch_chunk(std::uint64_t begin, const std::uint64_t end)
{
    QUILL_LOG_DEBUG(l_log_manager(), "Fetching chunk for index {} to {}", begin, end);
    quill::StopWatchTsc watch;
    std::vector<JournalEntry> entries;
    entries.reserve(end - begin);

    cache_.seek_to_index(begin);
    while (begin < end)
    {
        auto entry = fetch_entry(journal_.get());
        if (not match_dynamic_filter(entry))
        {
            entry.highlight = match_dynamic_highlighter(entry);
            entries.emplace_back(std::move(entry));
            begin++;
        }
        if (sd_journal_next(journal_.get()) <= 0)
        {
            break;
        }
    }
    QUILL_LOG_DEBUG(l_log_manager(), "Chunk fetched in {}", watch.elapsed_as<std::chrono::milliseconds>());
    return entries;
}

} // namespace jrn
