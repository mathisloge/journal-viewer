// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <async_journal_log_manager.hpp>
#include <entt/entity/registry.hpp>
#include <journal/lru_cache.hpp>
#include <journal_info.hpp>
#include <journal_instance_handle.hpp>
#include <journal_log_manager.hpp>

namespace jrn
{
struct JournalLogWindow
{
  public:
    explicit JournalLogWindow(entt::registry &registry,
                              std::string title,
                              JournalInstanceHandle handle,
                              const JournalInfo &info);
    void draw();
    void scroll_to_cursor(std::string_view cursor);

  private:
    void draw_priority_filter(std::string_view title, Priority priority);
    void draw_entry(int index, const JournalEntry &entry);

  private:
    entt::registry &registry_;
    std::string title_;
    bool open_{true};
    const JournalInfo &info_;
    AsyncJournalLogManager manager_;
    std::string selected_cursor_;
    std::optional<std::uint64_t> requested_scroll_index_;
    std::optional<std::uint64_t> past_scroll_index_;

    std::string search_text_;
    std::string exclude_text_;
};
} // namespace jrn
