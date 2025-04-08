#include "async_journal_log_manager.hpp"

namespace jrn
{
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
{}

void AsyncJournalLogManager::add_filter_systemd_unit(std::string systemd_unit)
{}

void AsyncJournalLogManager::remove_filter_systemd_unit(const std::string &systemd_unit)
{}

task<std::uint64_t> AsyncJournalLogManager::calculate_cursor_index(std::string_view cursor)
{}

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

} // namespace jrn
