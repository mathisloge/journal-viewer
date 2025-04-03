// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "journal_log_manager.hpp"
#include <utility>
#include "journal_instance.hpp"
namespace jrn
{
JournalLogManager::JournalLogManager(JournalInstanceHandle handle)
    : journal_{handle->create()}
{
    apply_current_matches();
}

void JournalLogManager::reset_filters()
{
    sd_journal_flush_matches(journal_.get());
    cache_.build_initial_cache(journal_.get());
}

void JournalLogManager::enable_priority(Priority priority)
{
    if (is_priority_enabled(priority))
    {
        return;
    }
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
    enabled_priorities_ &= ~details::flag_of(priority);
    apply_current_matches();
}

void JournalLogManager::add_filter_systemd_unit(std::string systemd_unit)
{
    if (not enabled_systemd_units_.emplace(std::move(systemd_unit)).second)
    {
        return;
    }
    apply_current_matches();
}

void JournalLogManager::remove_filter_systemd_unit(const std::string &systemd_unit)
{
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
    return false;
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

} // namespace jrn
