// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "journal_cursor_cache.hpp"
#include <journal/logger.hpp>
#include <quill/StopWatch.h>

DEFINE_LOGGER(cursor_cache);

namespace jrn
{
JournalCursorCache::JournalCursorCache(sd_journal *journal)
    : journal_{journal}
{
    cache_.reserve(10000);
}

void JournalCursorCache::build_initial_cache(sd_journal *journal)
{
    quill::StopWatchTsc watch;
    journal_ = journal;
    cache_.clear();
    const int r = sd_journal_seek_head(journal_);
    if (r < 0)
    {
        return;
    }
    cached_count_ = 0;
    continue_build_cache();
    QUILL_LOG_DEBUG(l_cursor_cache(), "Initial build time has taken {}", watch.elapsed_as<std::chrono::milliseconds>());
}

void JournalCursorCache::update_cache()
{
    quill::StopWatchTsc watch;
    auto last_key = cache_.back();
    const int r = sd_journal_seek_cursor(journal_, last_key.data());
    if (r < 0)
    {
        throw std::runtime_error("Can't seek to cursor");
    }
    continue_build_cache();
    QUILL_LOG_DEBUG(
        l_cursor_cache(), "continued build time has taken {}", watch.elapsed_as<std::chrono::milliseconds>());
}

void JournalCursorCache::seek_to_index(const std::uint64_t start_index)
{
    const std::uint64_t cache_index = start_index / cache_interval_;
    const auto cursor = cursor_at_index(cache_index);
    const int r = sd_journal_seek_cursor(journal_, cursor.cursor.cbegin());
    if (r < 0)
    {
        QUILL_LOG_CRITICAL(
            l_cursor_cache(), "Could not seek to cache index {}. Requested start = {}", cache_index, start_index);
        return;
    }
    const std::uint64_t skips = start_index - cursor.index;
    if (sd_journal_next_skip(journal_, skips) < 0)
    {
        QUILL_LOG_DEBUG(l_cursor_cache(), "Could not skip {}. Skipping to last entry.", skips);
        sd_journal_seek_tail(journal_);
        return;
    }
}

std::uint64_t JournalCursorCache::cached_count() const
{
    return cached_count_;
}

CursorCacheEntry JournalCursorCache::cursor_at_index(const std::uint64_t cache_index) const
{
    if (cache_index >= static_cast<std::uint64_t>(cache_.size()))
    {
        QUILL_LOG_DEBUG(
            l_cursor_cache(),
            "Could not find a cursor for chache index {}. Returning last index. Cache propably needs updating.",
            cache_index);
        return {.index = cache_.size() * cache_interval_, .cursor = cache_.back()};
    }
    return {.index = cache_index * cache_interval_, .cursor = cache_[cache_index]};
}

void JournalCursorCache::continue_build_cache()
{
    auto skipped = sd_journal_next(journal_);
    add_current_to_cache();
    while (skipped > 0)
    {
        cached_count_ += skipped;
        add_current_to_cache();
        skipped = sd_journal_next_skip(journal_, cache_interval_);
    }
}

bool JournalCursorCache::add_current_to_cache()
{
    char *cursor_data{nullptr};
    if (sd_journal_get_cursor(journal_, &cursor_data) < 0)
    {
        QUILL_LOG_ERROR(l_cursor_cache(), "could not get cursor at count {}", cached_count_);
        return false;
    }
    cache_.emplace_back(cursor_data);
    free(cursor_data);
    return true;
}
} // namespace jrn
