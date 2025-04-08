#pragma once
#include <asio/any_io_executor.hpp>
#include <asio/strand.hpp>
#include "async.hpp"
#include "journal_log_manager.hpp"
namespace jrn
{

class AsyncJournalLogManager
{
  public:
    explicit AsyncJournalLogManager(JournalInstanceHandle handle);
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

  private:
    void apply_current_matches();
    void add_priority_match(Priority priority);

  private:
    asio::strand<asio::any_io_executor> strand_;
    systemd::Journal journal_;
    JournalCache cache_{journal_.get()};
    LRUCache entry_cache_;
    std::uint8_t enabled_priorities_{std::numeric_limits<std::uint8_t>::max()};
    std::unordered_set<std::string> enabled_systemd_units_;
};

} // namespace jrn
