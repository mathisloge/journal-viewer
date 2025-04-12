// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <systemd/sd-journal.h>

namespace jrn
{
struct CursorCacheEntry
{
    std::uint64_t index;
    std::string_view cursor;
};
class JournalCursorCache
{
  public:
    JournalCursorCache(sd_journal *journal);
    void build_initial_cache(sd_journal *journal);
    void update_cache();
    void seek_to_index(const std::uint64_t start_index);
    std::uint64_t cached_count() const;

  private:
    CursorCacheEntry cursor_at_index(const std::uint64_t cache_index) const;
    void continue_build_cache();
    bool add_current_to_cache();

  private:
    std::uint64_t cache_interval_{1024};
    std::uint64_t cached_count_{};
    sd_journal *journal_;
    std::vector<std::string> cache_;
};
} // namespace jrn
